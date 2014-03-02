//===- CXTypes.h - Routines for manipulating CXTypes ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines routines for manipulating CXCursors.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_CXTYPES_H
#define LLVM_CLANG_CXTYPES_H

#include "clang-c/Index.h"
#include "clang/AST/Type.h"

namespace clang {
  
class ASTUnit;
  
namespace cxtype {
  
CXType MakeCXType(QualType T, CXTranslationUnit TU);

/// \brief Check recursively that all field types of \arg RD are valid.
/// \returns 0 if valid, CXTypeLayoutError_Incomplete if a field has an 
/// incomplete type, or CXTypeLayoutError_Dependent if a field has a depedent
/// type.
static inline long long visitRecordForValidation(const RecordDecl *RD) {
  for (RecordDecl::field_iterator I = RD->field_begin(), E = RD->field_end();
       I != E; ++I){
    QualType FQT = (*I)->getType();
    if (FQT->isIncompleteType())
      return CXTypeLayoutError_Incomplete;
    if (FQT->isDependentType())
      return CXTypeLayoutError_Dependent;
    // recurse
    if (const RecordType *ChildType = (*I)->getType()->getAs<RecordType>()) {
      if (const RecordDecl *Child = ChildType->getDecl()) {
        long long ret = visitRecordForValidation(Child);
        if (ret < 0)
          return ret;
      }
    }
    // else try next field
  }
  return 0;
}

/// \brief Check recursively that the record decl pointed by cursor \arg PC is
/// valid and that the record type \arg PT is valid and cehck recursively that 
/// all fields types are valid.
/// \returns 0 if valid or a negative value described in CXTypeLayoutError if
/// not valid.
static inline long long validateFieldParentType(CXCursor PC, CXType PT){
  if (clang_isInvalid(PC.kind))
    return CXTypeLayoutError_Invalid;
  const RecordDecl *RD =
        dyn_cast_or_null<RecordDecl>(cxcursor::getCursorDecl(PC));
  // validate parent declaration
  if (!RD || RD->isInvalidDecl())
    return CXTypeLayoutError_Invalid;
  RD = RD->getDefinition();
  if (!RD)
    return CXTypeLayoutError_Incomplete;
  if (RD->isInvalidDecl())
    return CXTypeLayoutError_Invalid;
  // validate parent type
  QualType RT = GetQualType(PT);
  if (RT->isIncompleteType())
    return CXTypeLayoutError_Incomplete;
  if (RT->isDependentType())
    return CXTypeLayoutError_Dependent;
  // We recurse into all record fields to detect incomplete and dependent types.
  long long Error = visitRecordForValidation(RD);
  if (Error < 0)
    return Error;
  return 0;
}

}} // end namespace clang::cxtype

#endif
