//===- unittests/Lex/PPCallbacksTest.cpp - PPCallbacks tests ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===--------------------------------------------------------------===//

#include "clang/Lex/Preprocessor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/HeaderSearchOptions.h"
#include "clang/Lex/ModuleLoader.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "clang/Parse/Parser.h"
#include "clang/Sema/Sema.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/Path.h"
#include "gtest/gtest.h"

using namespace llvm;
using namespace llvm::sys;
using namespace clang;

namespace {

// Stub out module loading.
class VoidModuleLoader : public ModuleLoader {
  virtual ModuleLoadResult loadModule(SourceLocation ImportLoc, 
                                      ModuleIdPath Path,
                                      Module::NameVisibilityKind Visibility,
                                      bool IsInclusionDirective) {
    return ModuleLoadResult();
  }

  virtual void makeModuleVisible(Module *Mod,
                                 Module::NameVisibilityKind Visibility,
                                 SourceLocation ImportLoc,
                                 bool Complain) { }
};

// Stub to collect data from InclusionDirective callbacks.
class InclusionDirectiveCallbacks : public PPCallbacks {
public:
  void InclusionDirective(SourceLocation HashLoc, 
    const Token &IncludeTok, 
    StringRef FileName, 
    bool IsAngled, 
    CharSourceRange FilenameRange, 
    const FileEntry *File, 
    StringRef SearchPath, 
    StringRef RelativePath, 
    const Module *Imported) {
      this->HashLoc = HashLoc;
      this->IncludeTok = IncludeTok;
      this->FileName = FileName.str();
      this->IsAngled = IsAngled;
      this->FilenameRange = FilenameRange;
      this->File = File;
      this->SearchPath = SearchPath.str();
      this->RelativePath = RelativePath.str();
      this->Imported = Imported;
  }

  SourceLocation HashLoc;
  Token IncludeTok;
  SmallString<16> FileName;
  bool IsAngled;
  CharSourceRange FilenameRange;
  const FileEntry* File;
  SmallString<16> SearchPath;
  SmallString<16> RelativePath;
  const Module* Imported;
};

// Stub to collect data from PragmaOpenCLExtension callbacks.
class PragmaOpenCLExtensionCallbacks : public PPCallbacks {
public:
  typedef struct {
    StringRef Name;
    unsigned State;
  } CallbackParameters;

  PragmaOpenCLExtensionCallbacks() : Name("Not called."), State(99) {};

  void PragmaOpenCLExtension(
    clang::SourceLocation NameLoc, const clang::IdentifierInfo *Name,
    clang::SourceLocation StateLoc, unsigned State) {
      this->NameLoc = NameLoc;
      this->Name = Name->getName().str();
      this->StateLoc = StateLoc;
      this->State = State;
  };

  SourceLocation NameLoc;
  StringRef Name;
  SourceLocation StateLoc;
  unsigned State;
};

// PPCallbacks test fixture.
class PPCallbacksTest : public ::testing::Test {
protected:
  PPCallbacksTest()
    : FileMgr(FileMgrOpts),
      DiagID(new DiagnosticIDs()),
      DiagOpts(new DiagnosticOptions()),
      Diags(DiagID, DiagOpts.getPtr(), new IgnoringDiagConsumer()),
      SourceMgr(Diags, FileMgr) {
    TargetOpts = new TargetOptions();
    TargetOpts->Triple = "x86_64-apple-darwin11.1.0";
    Target = TargetInfo::CreateTargetInfo(Diags, &*TargetOpts);
  }

  FileSystemOptions FileMgrOpts;
  FileManager FileMgr;
  IntrusiveRefCntPtr<DiagnosticIDs> DiagID;
  IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts;
  DiagnosticsEngine Diags;
  SourceManager SourceMgr;
  LangOptions LangOpts;
  IntrusiveRefCntPtr<TargetOptions> TargetOpts;
  IntrusiveRefCntPtr<TargetInfo> Target;

  // Register a header path as a known file and add its location
  // to search path.
  void AddFakeHeader(HeaderSearch& HeaderInfo, const char* HeaderPath, 
    bool IsSystemHeader) {
      // Tell FileMgr about header.
      FileMgr.getVirtualFile(HeaderPath, 0, 0);

      // Add header's parent path to search path.
      StringRef SearchPath = path::parent_path(HeaderPath);
      const DirectoryEntry *DE = FileMgr.getDirectory(SearchPath);
      DirectoryLookup DL(DE, SrcMgr::C_User, false);
      HeaderInfo.AddSearchPath(DL, IsSystemHeader);
  }

  // Get the raw source string of the range.
  StringRef GetSourceString(CharSourceRange Range) {
    const char* B = SourceMgr.getCharacterData(Range.getBegin());
    const char* E = SourceMgr.getCharacterData(Range.getEnd());

    return StringRef(B, E - B);
  }

  // Run lexer over SourceText and collect FilenameRange from
  // the InclusionDirective callback.
  CharSourceRange InclusionDirectiveFilenameRange(const char* SourceText, 
      const char* HeaderPath, bool SystemHeader) {
    MemoryBuffer *Buf = MemoryBuffer::getMemBuffer(SourceText);
    (void)SourceMgr.createMainFileIDForMemBuffer(Buf);

    VoidModuleLoader ModLoader;

    IntrusiveRefCntPtr<HeaderSearchOptions> HSOpts = new HeaderSearchOptions();
    HeaderSearch HeaderInfo(HSOpts, FileMgr, Diags, LangOpts, Target.getPtr());
    AddFakeHeader(HeaderInfo, HeaderPath, SystemHeader);

    IntrusiveRefCntPtr<PreprocessorOptions> PPOpts = new PreprocessorOptions();
    Preprocessor PP(PPOpts, Diags, LangOpts,
      Target.getPtr(),
      SourceMgr, HeaderInfo, ModLoader,
      /*IILookup =*/ 0,
      /*OwnsHeaderSearch =*/false,
      /*DelayInitialization =*/ false);
    InclusionDirectiveCallbacks* Callbacks = new InclusionDirectiveCallbacks;
    PP.addPPCallbacks(Callbacks); // Takes ownership.

    // Lex source text.
    PP.EnterMainSourceFile();

    while (true) {
      Token Tok;
      PP.Lex(Tok);
      if (Tok.is(tok::eof))
        break;
    }

    // Callbacks have been executed at this point -- return filename range.
    return Callbacks->FilenameRange;
  }

  PragmaOpenCLExtensionCallbacks::CallbackParameters 
  PragmaOpenCLExtensionCall(const char* SourceText) {
    LangOptions OpenCLLangOpts;
    OpenCLLangOpts.OpenCL = 1;

    MemoryBuffer* sourceBuf = MemoryBuffer::getMemBuffer(SourceText, "test.cl");
    (void)SourceMgr.createMainFileIDForMemBuffer(sourceBuf);

    VoidModuleLoader ModLoader;
    HeaderSearch HeaderInfo(new HeaderSearchOptions, FileMgr, Diags, 
                            OpenCLLangOpts, Target.getPtr());

    Preprocessor PP(new PreprocessorOptions(), Diags, OpenCLLangOpts, 
                    Target.getPtr(),
                    SourceMgr, HeaderInfo, ModLoader,
                   /*IILookup =*/ 0,
                    /*OwnsHeaderSearch =*/false,
                    /*DelayInitialization =*/ false);

    // parser actually sets correct pragma handlers for preprocessor
    // according to LangOptions, so we init Parser to register opencl
    // pragma handlers
    ASTContext Context(OpenCLLangOpts, SourceMgr, Target.getPtr(), 
                       PP.getIdentifierTable(), PP.getSelectorTable(), 
                       PP.getBuiltinInfo(), 0);    
    ASTConsumer Consumer;
    Sema S(PP, Context, Consumer);
    Parser P(PP, S, false);
    PragmaOpenCLExtensionCallbacks* Callbacks = new PragmaOpenCLExtensionCallbacks;
    PP.addPPCallbacks(Callbacks); // Takes ownership.

    // Lex source text.
    PP.EnterMainSourceFile();
    while (true) {
      Token Tok;
      PP.Lex(Tok);
      if (Tok.is(tok::eof))
        break;
    }

    PragmaOpenCLExtensionCallbacks::CallbackParameters RetVal = {
      Callbacks->Name.str(),
      Callbacks->State
    };
    return RetVal;    
  }
};

TEST_F(PPCallbacksTest, QuotedFilename) {
  const char* Source =
    "#include \"quoted.h\"\n";

  CharSourceRange Range =
    InclusionDirectiveFilenameRange(Source, "/quoted.h", false);

  ASSERT_EQ("\"quoted.h\"", GetSourceString(Range));
}

TEST_F(PPCallbacksTest, AngledFilename) {
  const char* Source =
    "#include <angled.h>\n";

  CharSourceRange Range =
    InclusionDirectiveFilenameRange(Source, "/angled.h", true);

  ASSERT_EQ("<angled.h>", GetSourceString(Range));
}

TEST_F(PPCallbacksTest, QuotedInMacro) {
  const char* Source =
    "#define MACRO_QUOTED \"quoted.h\"\n"
    "#include MACRO_QUOTED\n";

  CharSourceRange Range =
    InclusionDirectiveFilenameRange(Source, "/quoted.h", false);

  ASSERT_EQ("\"quoted.h\"", GetSourceString(Range));
}

TEST_F(PPCallbacksTest, AngledInMacro) {
  const char* Source =
    "#define MACRO_ANGLED <angled.h>\n"
    "#include MACRO_ANGLED\n";

  CharSourceRange Range =
    InclusionDirectiveFilenameRange(Source, "/angled.h", true);

  ASSERT_EQ("<angled.h>", GetSourceString(Range));
}

TEST_F(PPCallbacksTest, StringizedMacroArgument) {
  const char* Source =
    "#define MACRO_STRINGIZED(x) #x\n"
    "#include MACRO_STRINGIZED(quoted.h)\n";

  CharSourceRange Range =
    InclusionDirectiveFilenameRange(Source, "/quoted.h", false);

  ASSERT_EQ("\"quoted.h\"", GetSourceString(Range));
}

TEST_F(PPCallbacksTest, ConcatenatedMacroArgument) {
  const char* Source =
    "#define MACRO_ANGLED <angled.h>\n"
    "#define MACRO_CONCAT(x, y) x ## _ ## y\n"
    "#include MACRO_CONCAT(MACRO, ANGLED)\n";

  CharSourceRange Range =
    InclusionDirectiveFilenameRange(Source, "/angled.h", false);

  ASSERT_EQ("<angled.h>", GetSourceString(Range));
}

TEST_F(PPCallbacksTest, TrigraphFilename) {
  const char* Source =
    "#include \"tri\?\?-graph.h\"\n";

  CharSourceRange Range =
    InclusionDirectiveFilenameRange(Source, "/tri~graph.h", false);

  ASSERT_EQ("\"tri\?\?-graph.h\"", GetSourceString(Range));
}

TEST_F(PPCallbacksTest, TrigraphInMacro) {
  const char* Source =
    "#define MACRO_TRIGRAPH \"tri\?\?-graph.h\"\n"
    "#include MACRO_TRIGRAPH\n";

  CharSourceRange Range =
    InclusionDirectiveFilenameRange(Source, "/tri~graph.h", false);

  ASSERT_EQ("\"tri\?\?-graph.h\"", GetSourceString(Range));
}

TEST_F(PPCallbacksTest, OpenCLExtensionPragmaEnabled) {
  const char* Source =
    "#pragma OPENCL EXTENSION cl_khr_fp64 : enable\n";

  PragmaOpenCLExtensionCallbacks::CallbackParameters Parameters =
    PragmaOpenCLExtensionCall(Source);

  ASSERT_EQ("cl_khr_fp64", Parameters.Name);
  unsigned ExpectedState = 1;
  ASSERT_EQ(ExpectedState, Parameters.State);
}

TEST_F(PPCallbacksTest, OpenCLExtensionPragmaDisabled) {
  const char* Source =
    "#pragma OPENCL EXTENSION cl_khr_fp16 : disable\n";

  PragmaOpenCLExtensionCallbacks::CallbackParameters Parameters =
    PragmaOpenCLExtensionCall(Source);

  ASSERT_EQ("cl_khr_fp16", Parameters.Name);
  unsigned ExpectedState = 0;
  ASSERT_EQ(ExpectedState, Parameters.State);
}

} // anonoymous namespace
