// from SemaCXX/class-layout.cpp
// RUN: c-index-test -test-print-type %s -target x86_64-pc-linux-gnu | FileCheck -check-prefix=CHECK64 %s
// RUN: c-index-test -test-print-type %s -target i386-apple-darwin9 | FileCheck -check-prefix=CHECK32 %s

namespace basic {

// CHECK64: VarDecl=v:[[@LINE+2]]:6 (Definition) [type=void] [typekind=Void] [isPOD=0]
// CHECK32: VarDecl=v:[[@LINE+1]]:6 (Definition) [type=void] [typekind=Void] [isPOD=0]
void v;

// CHECK64: VarDecl=v1:[[@LINE+2]]:7 (Definition) [type=void *] [typekind=Pointer] [sizeof=8] [alignof=8] [isPOD=1]
// CHECK32: VarDecl=v1:[[@LINE+1]]:7 (Definition) [type=void *] [typekind=Pointer] [sizeof=4] [alignof=4] [isPOD=1]
void *v1;

// CHECK64: StructDecl=simple:[[@LINE+2]]:8 (Definition) [type=basic::simple] [typekind=Record] [sizeof=48] [alignof=8] [isPOD=1]
// CHECK32: StructDecl=simple:[[@LINE+1]]:8 (Definition) [type=basic::simple] [typekind=Record] [sizeof=36] [alignof=4] [isPOD=1]
struct simple {
  int a;
  char b;
  int c:3;
  long d;
  int e:5;
  int f:4;
// CHECK64: FieldDecl=g:[[@LINE+2]]:13 (Definition) [type=long long] [typekind=LongLong] [sizeof=8] [alignof=8] [offset=192] [isPOD=1]
// CHECK32: FieldDecl=g:[[@LINE+1]]:13 (Definition) [type=long long] [typekind=LongLong] [sizeof=8] [alignof=4] [offset=128] [isPOD=1]
  long long g;
  char h:3;
  char i:3;
  float j;
// CHECK64: FieldDecl=k:[[@LINE+2]]:10 (Definition) [type=char *] [typekind=Pointer] [sizeof=8] [alignof=8] [offset=320] [isPOD=1]
// CHECK32: FieldDecl=k:[[@LINE+1]]:10 (Definition) [type=char *] [typekind=Pointer] [sizeof=4] [alignof=4] [offset=256] [isPOD=1]
  char * k;
};

union u {
  int u1;
  long long u2;
  struct simple s1;
};

}

namespace Incomplete {

// expect compilation error, not crash.
union f {
  struct forward_decl f1;
};

class A;
class B {
  A* a1;
  A& a2;
};

class C;
class D {
  C c1;
};
class C {
  int c;
};

}

namespace Sizes {

// CHECK64: StructDecl=A:[[@LINE+2]]:8 (Definition) [type=Sizes::A] [typekind=Record] [sizeof=8] [alignof=4] [isPOD=1]
// CHECK32: StructDecl=A:[[@LINE+1]]:8 (Definition) [type=Sizes::A] [typekind=Record] [sizeof=8] [alignof=4] [isPOD=1]
struct A {
  int a;
  char b;
};

// CHECK64: StructDecl=B:[[@LINE+2]]:8 (Definition) [type=Sizes::B] [typekind=Record] [sizeof=12] [alignof=4] [isPOD=0]
// CHECK32: StructDecl=B:[[@LINE+1]]:8 (Definition) [type=Sizes::B] [typekind=Record] [sizeof=12] [alignof=4] [isPOD=0]
struct B : A {
  char c;
};

// CHECK64: StructDecl=C:[[@LINE+2]]:8 (Definition) [type=Sizes::C] [typekind=Record] [sizeof=8] [alignof=4] [isPOD=0]
// CHECK32: StructDecl=C:[[@LINE+1]]:8 (Definition) [type=Sizes::C] [typekind=Record] [sizeof=8] [alignof=4] [isPOD=0]
struct C {
// Make fields private so C won't be a POD type.
private:
  int a;
  char b;
};

// CHECK64: StructDecl=D:[[@LINE+2]]:8 (Definition) [type=Sizes::D] [typekind=Record] [sizeof=8] [alignof=4] [isPOD=0]
// CHECK32: StructDecl=D:[[@LINE+1]]:8 (Definition) [type=Sizes::D] [typekind=Record] [sizeof=8] [alignof=4] [isPOD=0]
struct D : C {
  char c;
};

// CHECK64: StructDecl=E:[[@LINE+2]]:32 (Definition) [type=Sizes::E] [typekind=Record] [sizeof=5] [alignof=1] [isPOD=1]
// CHECK32: StructDecl=E:[[@LINE+1]]:32 (Definition) [type=Sizes::E] [typekind=Record] [sizeof=5] [alignof=1] [isPOD=1]
struct __attribute__((packed)) E {
  char b;
  int a;
};

// CHECK64: StructDecl=F:[[@LINE+2]]:32 (Definition) [type=Sizes::F] [typekind=Record] [sizeof=6] [alignof=1] [isPOD=0]
// CHECK32: StructDecl=F:[[@LINE+1]]:32 (Definition) [type=Sizes::F] [typekind=Record] [sizeof=6] [alignof=1] [isPOD=0]
struct __attribute__((packed)) F : E {
  char d;
};

struct G { G(); };
// CHECK64: StructDecl=H:[[@LINE+2]]:8 (Definition) [type=Sizes::H] [typekind=Record] [sizeof=1] [alignof=1] [isPOD=0]
// CHECK32: StructDecl=H:[[@LINE+1]]:8 (Definition) [type=Sizes::H] [typekind=Record] [sizeof=1] [alignof=1] [isPOD=0]
struct H : G { };

// CHECK64: StructDecl=I:[[@LINE+2]]:8 (Definition) [type=Sizes::I] [typekind=Record] [sizeof=5] [alignof=1] [isPOD=1]
// CHECK32: StructDecl=I:[[@LINE+1]]:8 (Definition) [type=Sizes::I] [typekind=Record] [sizeof=5] [alignof=1] [isPOD=1]
struct I {
  char b;
  int a;
} __attribute__((packed));

}

// PR5580
namespace PR5580 {

// CHECK64: ClassDecl=A:[[@LINE+2]]:7 (Definition) [type=PR5580::A] [typekind=Record] [sizeof=1] [alignof=1] [isPOD=0]
// CHECK32: ClassDecl=A:[[@LINE+1]]:7 (Definition) [type=PR5580::A] [typekind=Record] [sizeof=1] [alignof=1] [isPOD=0]
class A { bool iv0 : 1; };

// CHECK64: ClassDecl=B:[[@LINE+2]]:7 (Definition) [type=PR5580::B] [typekind=Record] [sizeof=2] [alignof=1] [isPOD=0]
// CHECK32: ClassDecl=B:[[@LINE+1]]:7 (Definition) [type=PR5580::B] [typekind=Record] [sizeof=2] [alignof=1] [isPOD=0]
class B : A { bool iv0 : 1; };

// CHECK64: StructDecl=C:[[@LINE+2]]:8 (Definition) [type=PR5580::C] [typekind=Record] [sizeof=1] [alignof=1] [isPOD=1]
// CHECK32: StructDecl=C:[[@LINE+1]]:8 (Definition) [type=PR5580::C] [typekind=Record] [sizeof=1] [alignof=1] [isPOD=1]
struct C { bool iv0 : 1; };

// CHECK64: StructDecl=D:[[@LINE+2]]:8 (Definition) [type=PR5580::D] [typekind=Record] [sizeof=2] [alignof=1] [isPOD=0]
// CHECK32: StructDecl=D:[[@LINE+1]]:8 (Definition) [type=PR5580::D] [typekind=Record] [sizeof=2] [alignof=1] [isPOD=0]
struct D : C { bool iv0 : 1; };

}

namespace Test1 {

// Test that we don't assert on this hierarchy.
struct A { };
struct B : A { virtual void b(); };
class C : virtual A { int c; };
struct D : virtual B { };
struct E : C, virtual D { };
class F : virtual E { };
// CHECK64: StructDecl=G:[[@LINE+2]]:8 (Definition) [type=Test1::G] [typekind=Record] [sizeof=24] [alignof=8] [isPOD=0]
// CHECK32: StructDecl=G:[[@LINE+1]]:8 (Definition) [type=Test1::G] [typekind=Record] [sizeof=16] [alignof=4] [isPOD=0]
struct G : virtual E, F { };

}

namespace Test2 {

// Test that this somewhat complex class structure is laid out correctly.
struct A { };
struct B : A { virtual void b(); };
struct C : virtual B { };
struct D : virtual A { };
struct E : virtual B, D { };
struct F : E, virtual C { };
struct G : virtual F, A { };
// CHECK64: StructDecl=H:[[@LINE+2]]:8 (Definition) [type=Test2::H] [typekind=Record] [sizeof=24] [alignof=8] [isPOD=0]
// CHECK32: StructDecl=H:[[@LINE+1]]:8 (Definition) [type=Test2::H] [typekind=Record] [sizeof=12] [alignof=4] [isPOD=0]
struct H { G g; };

}


namespace Test3 {
// CHECK64: ClassDecl=B:[[@LINE+2]]:7 (Definition) [type=Test3::B] [typekind=Record] [sizeof=16] [alignof=8] [isPOD=0]
// CHECK32: ClassDecl=B:[[@LINE+1]]:7 (Definition) [type=Test3::B] [typekind=Record] [sizeof=8] [alignof=4] [isPOD=0]
class B {
public:
  virtual void b(){}
// CHECK64: FieldDecl=b_field:[[@LINE+2]]:8 (Definition) [type=long] [typekind=Long] [sizeof=8] [alignof=8] [offset=64] [isPOD=1]
// CHECK32: FieldDecl=b_field:[[@LINE+1]]:8 (Definition) [type=long] [typekind=Long] [sizeof=4] [alignof=4] [offset=32] [isPOD=1]
  long b_field;
protected:
private:
};

// CHECK32: ClassDecl=A:[[@LINE+1]]:7 (Definition) [type=Test3::A] [typekind=Record] [sizeof=16] [alignof=4] [isPOD=0]
class A : public B {
public:
// CHECK64: FieldDecl=a_field:[[@LINE+2]]:7 (Definition) [type=int] [typekind=Int] [sizeof=4] [alignof=4] [offset=128] [isPOD=1]
// CHECK32: FieldDecl=a_field:[[@LINE+1]]:7 (Definition) [type=int] [typekind=Int] [sizeof=4] [alignof=4] [offset=64] [isPOD=1]
  int a_field;
  virtual void a(){}
// CHECK64: FieldDecl=one:[[@LINE+2]]:8 (Definition) [type=char] [typekind=Char_S] [sizeof=1] [alignof=1] [offset=160] [isPOD=1]
// CHECK32: FieldDecl=one:[[@LINE+1]]:8 (Definition) [type=char] [typekind=Char_S] [sizeof=1] [alignof=1] [offset=96] [isPOD=1]
  char one;
protected:
private:
};

// CHECK64: ClassDecl=D:[[@LINE+2]]:7 (Definition) [type=Test3::D] [typekind=Record] [sizeof=16] [alignof=8] [isPOD=0]
// CHECK32: ClassDecl=D:[[@LINE+1]]:7 (Definition) [type=Test3::D] [typekind=Record] [sizeof=12] [alignof=4] [isPOD=0]
class D {
public:
  virtual void b(){}
// CHECK64: FieldDecl=a:[[@LINE+2]]:10 (Definition) [type=double] [typekind=Double] [sizeof=8] [alignof=8] [offset=64] [isPOD=1]
// CHECK32: FieldDecl=a:[[@LINE+1]]:10 (Definition) [type=double] [typekind=Double] [sizeof=8] [alignof=4] [offset=32] [isPOD=1]
  double a;
};

// CHECK64: ClassDecl=C:[[@LINE+2]]:7 (Definition) [type=Test3::C] [typekind=Record] [sizeof=88] [alignof=8] [isPOD=0]
// CHECK32: ClassDecl=C:[[@LINE+1]]:7 (Definition) [type=Test3::C] [typekind=Record] [sizeof=60] [alignof=4] [isPOD=0]
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
// CHECK64: FieldDecl=fg:[[@LINE+2]]:7 (Definition) [type=Test3::C] [typekind=Record] [sizeof=88] [alignof=8] [offset=128] [isPOD=0]
// CHECK32: FieldDecl=fg:[[@LINE+1]]:7 (Definition) [type=Test3::C] [typekind=Record] [sizeof=60] [alignof=4] [offset=96] [isPOD=0]
    C fg;
// CHECK64: FieldDecl=rg:[[@LINE+2]]:8 (Definition) [type=Test3::C &] [typekind=LValueReference] [sizeof=88] [alignof=8] [offset=832] [isPOD=0]
// CHECK32: FieldDecl=rg:[[@LINE+1]]:8 (Definition) [type=Test3::C &] [typekind=LValueReference] [sizeof=60] [alignof=4] [offset=576] [isPOD=0]
    C &rg;
    int x;
};

struct DerivedStruct : public BaseStruct { int x; };
struct G { int g_field; };
struct H : public G, public virtual D {};

// CHECK64: StructDecl=I:[[@LINE+2]]:8 (Definition) [type=Test3::I] [typekind=Record] [sizeof=32] [alignof=8] [isPOD=0]
// CHECK32: StructDecl=I:[[@LINE+1]]:8 (Definition) [type=Test3::I] [typekind=Record] [sizeof=24] [alignof=4] [isPOD=0]
struct I : public virtual D
{
  virtual ~I(){}
  double q;
};

struct K { int k; };

struct L { int l; };

struct M : public virtual K { int m; };

// CHECK64: StructDecl=N:[[@LINE+2]]:8 (Definition) [type=Test3::N] [typekind=Record] [sizeof=24] [alignof=8] [isPOD=0]
// CHECK32: StructDecl=N:[[@LINE+1]]:8 (Definition) [type=Test3::N] [typekind=Record] [sizeof=16] [alignof=4] [isPOD=0]
struct N : public L, public M { virtual void f(){} };

// CHECK64: StructDecl=O:[[@LINE+2]]:8 (Definition) [type=Test3::O] [typekind=Record] [sizeof=32] [alignof=8] [isPOD=0]
// CHECK32: StructDecl=O:[[@LINE+1]]:8 (Definition) [type=Test3::O] [typekind=Record] [sizeof=24] [alignof=4] [isPOD=0]
struct O : public H, public G { virtual void fo(){} };

// CHECK64: StructDecl=P:[[@LINE+2]]:8 (Definition) [type=Test3::P] [typekind=Record] [sizeof=24] [alignof=8] [isPOD=0]
// CHECK32: StructDecl=P:[[@LINE+1]]:8 (Definition) [type=Test3::P] [typekind=Record] [sizeof=20] [alignof=4] [isPOD=0]
struct P : public M, public virtual L { int p; };

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

struct f { virtual int asd() {return -90;} };

struct s : public virtual f {
  virtual ~s(){}
  int r;
  virtual int asd() {return -9;}
};

// CHECK64: StructDecl=sd:[[@LINE+2]]:8 (Definition) [type=Test3::sd] [typekind=Record] [sizeof=32] [alignof=8] [isPOD=0]
// CHECK32: StructDecl=sd:[[@LINE+1]]:8 (Definition) [type=Test3::sd] [typekind=Record] [sizeof=20] [alignof=4] [isPOD=0]
struct sd : virtual s, virtual ICh {
  virtual ~sd(){}
// CHECK64: FieldDecl=q:[[@LINE+2]]:7 (Definition) [type=int] [typekind=Int] [sizeof=4] [alignof=4] [offset=64] [isPOD=1]
// CHECK32: FieldDecl=q:[[@LINE+1]]:7 (Definition) [type=int] [typekind=Int] [sizeof=4] [alignof=4] [offset=32] [isPOD=1]
  int q;
// CHECK64: FieldDecl=y:[[@LINE+2]]:8 (Definition) [type=char] [typekind=Char_S] [sizeof=1] [alignof=1] [offset=96] [isPOD=1]
// CHECK32: FieldDecl=y:[[@LINE+1]]:8 (Definition) [type=char] [typekind=Char_S] [sizeof=1] [alignof=1] [offset=64] [isPOD=1]
  char y;
  virtual int asd() {return -1;}
};

// CHECK64: StructDecl=AV:[[@LINE+2]]:8 (Definition) [type=Test3::AV] [typekind=Record] [sizeof=8] [alignof=8] [isPOD=0]
// CHECK32: StructDecl=AV:[[@LINE+1]]:8 (Definition) [type=Test3::AV] [typekind=Record] [sizeof=4] [alignof=4] [isPOD=0]
struct AV { virtual void foo(); };
// CHECK64: StructDecl=BV:[[@LINE+2]]:8 (Definition) [type=Test3::BV] [typekind=Record] [sizeof=8] [alignof=8] [isPOD=0]
// CHECK32: StructDecl=BV:[[@LINE+1]]:8 (Definition) [type=Test3::BV] [typekind=Record] [sizeof=4] [alignof=4] [isPOD=0]
struct BV : AV {};
// CHECK64: StructDecl=CV:[[@LINE+2]]:8 (Definition) [type=Test3::CV] [typekind=Record] [sizeof=8] [alignof=8] [isPOD=0]
// CHECK32: StructDecl=CV:[[@LINE+1]]:8 (Definition) [type=Test3::CV] [typekind=Record] [sizeof=4] [alignof=4] [isPOD=0]
struct CV : virtual BV {
  CV();
  virtual void foo();
};
// CHECK64: StructDecl=DV:[[@LINE+2]]:8 (Definition) [type=Test3::DV] [typekind=Record] [sizeof=8] [alignof=8] [isPOD=0]
// CHECK32: StructDecl=DV:[[@LINE+1]]:8 (Definition) [type=Test3::DV] [typekind=Record] [sizeof=4] [alignof=4] [isPOD=0]
struct DV : BV {};
// CHECK64: StructDecl=EV:[[@LINE+2]]:8 (Definition) [type=Test3::EV] [typekind=Record] [sizeof=16] [alignof=8] [isPOD=0]
// CHECK32: StructDecl=EV:[[@LINE+1]]:8 (Definition) [type=Test3::EV] [typekind=Record] [sizeof=8] [alignof=4] [isPOD=0]
struct EV : CV, DV {};

}

