// from Sema/ms_class_layout.cpp
//
#pragma pack(push, 8)

class B {
public:
  virtual void b(){}
  int b_field;
protected:
private:
};

class A : public B {
public:
  int a_field;
  virtual void a(){}
  char one;
protected:
private:
};

class D {
public:
  virtual void b(){}
  double a;
};

class C : public virtual A,
          public D, public B {
public:
  double c1_field;
  int c2_field;
  double c3_field;
  int c4_field;
  virtual void foo(){}
  virtual void bar(){}
protected:
private:
};

struct BaseStruct
{
    BaseStruct(){}
    double v0;
    float v1;
    C fg;
};

struct DerivedStruct : public BaseStruct {
  int x;
};

struct G
{
    int g_field;
};

struct H : public G,
           public virtual D
{
};

struct I : public virtual D
{
  virtual ~I(){}
  double q;
};

struct K
{
  int k;
};

struct L
{
  int l;
};

struct M : public virtual K
{
  int m;
};

struct N : public L, public M
{
  virtual void f(){}
};

struct O : public H, public G {
  virtual void fo(){}
};

struct P : public M, public virtual L {
  int p;
};

struct R {};

class IA {
public:
  virtual ~IA(){}
  virtual void ia() = 0;
};

class ICh : public virtual IA {
public:
  virtual ~ICh(){}
  virtual void ia(){}
  virtual void iCh(){}
};

struct f {
  virtual int asd() {return -90;}
};

struct s : public virtual f {
  virtual ~s(){}
  int r;
  virtual int asd() {return -9;}
};

struct sd : virtual s, virtual ICh {
  virtual ~sd(){}
  int q;
  char y;
  virtual int asd() {return -1;}
};
struct AV {
  virtual void foo();
};
struct BV : AV {
};
struct CV : virtual BV {
  CV();
  virtual void foo();
};
struct DV : BV {
};
struct EV : CV, DV {
};
#pragma pack(pop)

// This needs only for building layouts.
// Without this clang doesn`t dump record layouts.
int main() {
  // This avoid "Can't yet mangle constructors!" for MS ABI.
  C* c;
  c->foo();
  DerivedStruct* v;
  H* g;
  BaseStruct* u;
  I* i;
  N* n;
  O* o;
  P* p;
  R* r;
  sd *h;
  EV *j;
  return 0;
}

// Overriding a method means that all the vbases containing that
// method need a vtordisp.
namespace test1 {
  struct A { virtual void foo(); };
  struct B : A {};
  struct C : virtual A, virtual B { C(); virtual void foo(); };
  void test() { C *c; }
}

// RUN: c-index-test -test-print-typekind %s -m64 | FileCheck -check-prefix=CHECK64 %s
// CHECK64: ClassDecl=B:5:7 (Definition) typekind=Record [size=16] [alignment=8] [isPOD=0]
// CHECK64: FieldDecl=b_field:8:7 (Definition) typekind=Int [offset=64] [isPOD=1]
// CHECK64: ClassDecl=A:13:7 (Definition) typekind=Record [size=24] [alignment=8] [isPOD=0]
// CHECK64: FieldDecl=a_field:15:7 (Definition) typekind=Int [offset=96] [isPOD=1]
// CHECK64: FieldDecl=one:17:8 (Definition) typekind=Char_S [offset=128] [isPOD=1]
// CHECK64: ClassDecl=D:22:7 (Definition) typekind=Record [size=16] [alignment=8] [isPOD=0]
// CHECK64: FieldDecl=a:25:10 (Definition) typekind=Double [offset=64] [isPOD=1]
// CHECK64: ClassDecl=C:28:7 (Definition) typekind=Record [size=88] [alignment=8] [isPOD=0]
// CHECK64: FieldDecl=c1_field:31:10 (Definition) typekind=Double [offset=256] [isPOD=1]
// CHECK64: FieldDecl=c2_field:32:7 (Definition) typekind=Int [offset=320] [isPOD=1]
// CHECK64: FieldDecl=c3_field:33:10 (Definition) typekind=Double [offset=384] [isPOD=1]
// CHECK64: FieldDecl=c4_field:34:7 (Definition) typekind=Int [offset=448] [isPOD=1]
// CHECK64: StructDecl=BaseStruct:41:8 (Definition) typekind=Record [size=104] [alignment=8] [isPOD=0]
// CHECK64: FieldDecl=v0:44:12 (Definition) typekind=Double [offset=0] [isPOD=1]
// CHECK64: FieldDecl=v1:45:11 (Definition) typekind=Float [offset=64] [isPOD=1]
// CHECK64: FieldDecl=fg:46:7 (Definition) typekind=Record [size=88] [alignment=8] [offset=128] [isPOD=0]
// CHECK64: StructDecl=DerivedStruct:49:8 (Definition) typekind=Record [size=112] [alignment=8] [isPOD=0]
// CHECK64: FieldDecl=x:50:7 (Definition) typekind=Int [offset=832] [isPOD=1]
// CHECK64: StructDecl=G:53:8 (Definition) typekind=Record [size=4] [alignment=4] [isPOD=1]
// CHECK64: FieldDecl=g_field:55:9 (Definition) typekind=Int [offset=0] [isPOD=1]
// CHECK64: StructDecl=H:58:8 (Definition) typekind=Record [size=32] [alignment=8] [isPOD=0]
// CHECK64: StructDecl=I:63:8 (Definition) typekind=Record [size=32] [alignment=8] [isPOD=0]
// CHECK64: FieldDecl=q:66:10 (Definition) typekind=Double [offset=64] [isPOD=1]
// CHECK64: StructDecl=K:69:8 (Definition) typekind=Record [size=4] [alignment=4] [isPOD=1]
// CHECK64: FieldDecl=k:71:7 (Definition) typekind=Int [offset=0] [isPOD=1]
// CHECK64: StructDecl=L:74:8 (Definition) typekind=Record [size=4] [alignment=4] [isPOD=1]
// CHECK64: FieldDecl=l:76:7 (Definition) typekind=Int [offset=0] [isPOD=1]
// CHECK64: StructDecl=M:79:8 (Definition) typekind=Record [size=16] [alignment=8] [isPOD=0]
// CHECK64: FieldDecl=m:81:7 (Definition) typekind=Int [offset=64] [isPOD=1]
// CHECK64: StructDecl=N:84:8 (Definition) typekind=Record [size=24] [alignment=8] [isPOD=0]
// CHECK64: StructDecl=O:89:8 (Definition) typekind=Record [size=32] [alignment=8] [isPOD=0]
// CHECK64: StructDecl=P:93:8 (Definition) typekind=Record [size=24] [alignment=8] [isPOD=0]
// CHECK64: FieldDecl=p:94:7 (Definition) typekind=Int [offset=96] [isPOD=1]
// CHECK64: StructDecl=R:97:8 (Definition) typekind=Record [size=1] [alignment=1] [isPOD=1]
// CHECK64: ClassDecl=IA:99:7 (Definition) typekind=Record [size=8] [alignment=8] [isPOD=0]
// CHECK64: ClassDecl=ICh:105:7 (Definition) typekind=Record [size=8] [alignment=8] [isPOD=0]
// CHECK64: StructDecl=f:112:8 (Definition) typekind=Record [size=8] [alignment=8] [isPOD=0]
// CHECK64: StructDecl=s:116:8 (Definition) typekind=Record [size=16] [alignment=8] [isPOD=0]
// CHECK64: FieldDecl=r:118:7 (Definition) typekind=Int [offset=64] [isPOD=1]
// CHECK64: StructDecl=sd:122:8 (Definition) typekind=Record [size=32] [alignment=8] [isPOD=0]
// CHECK64: FieldDecl=q:124:7 (Definition) typekind=Int [offset=64] [isPOD=1]
// CHECK64: FieldDecl=y:125:8 (Definition) typekind=Char_S [offset=96] [isPOD=1]
// CHECK64: StructDecl=AV:128:8 (Definition) typekind=Record [size=8] [alignment=8] [isPOD=0]
// CHECK64: StructDecl=BV:131:8 (Definition) typekind=Record [size=8] [alignment=8] [isPOD=0]
// CHECK64: StructDecl=CV:133:8 (Definition) typekind=Record [size=8] [alignment=8] [isPOD=0]
// CHECK64: StructDecl=DV:137:8 (Definition) typekind=Record [size=8] [alignment=8] [isPOD=0]
// CHECK64: StructDecl=EV:139:8 (Definition) typekind=Record [size=16] [alignment=8] [isPOD=0]
// CHECK64: StructDecl=A:165:10 (Definition) typekind=Record [size=8] [alignment=8] [isPOD=0]
// CHECK64: StructDecl=B:166:10 (Definition) typekind=Record [size=8] [alignment=8] [isPOD=0]
// CHECK64: StructDecl=C:167:10 (Definition) typekind=Record [size=16] [alignment=8] [isPOD=0]

