// from SemaCXX/class-layout.cpp
// RUN: c-index-test -test-print-typekind %s -target x86_64-pc-linux-gnu | FileCheck -check-prefix=CHECK64 %s
// RUN: c-index-test -test-print-typekind %s -target i386-apple-darwin9 | FileCheck -check-prefix=CHECK32 %s

// CHECK64: StructDecl=simple:[[@LINE+2]]:8 (Definition) typekind=Record [size=384] [alignment=64] [isPOD=1]
// CHECK32: StructDecl=simple:[[@LINE+1]]:8 (Definition) typekind=Record [size=288] [alignment=32] [isPOD=1]
struct simple {
  int a;
  char b;
  int c:3;
  long d;
  int e:5;
  int f:4;
// CHECK64: FieldDecl=g:[[@LINE+2]]:13 (Definition) typekind=LongLong [offset=192] [isPOD=1]
// CHECK32: FieldDecl=g:[[@LINE+1]]:13 (Definition) typekind=LongLong [offset=128] [isPOD=1]
  long long g;
  char h:3;
  char i:3;
  float j;
// CHECK64: FieldDecl=k:[[@LINE+2]]:10 (Definition) typekind=Pointer [offset=320] [isPOD=1]
// CHECK32: FieldDecl=k:[[@LINE+1]]:10 (Definition) typekind=Pointer [offset=256] [isPOD=1]
  char * k;
};

union u {
  int u1;
  long long u2;
  struct simple s1;
};

// expect compilation error, not crash.
union f {
  struct forward_decl f1;
};

// CHECK64: StructDecl=A:[[@LINE+2]]:8 (Definition) typekind=Record [size=64] [alignment=32] [isPOD=1]
// CHECK32: StructDecl=A:[[@LINE+1]]:8 (Definition) typekind=Record [size=64] [alignment=32] [isPOD=1]
struct A {
  int a;
  char b;
};

// CHECK64: StructDecl=B:[[@LINE+2]]:8 (Definition) typekind=Record [size=96] [alignment=32] [isPOD=0]
// CHECK32: StructDecl=B:[[@LINE+1]]:8 (Definition) typekind=Record [size=96] [alignment=32] [isPOD=0]
struct B : A {
  char c;
};

// CHECK64: StructDecl=C:[[@LINE+2]]:8 (Definition) typekind=Record [size=64] [alignment=32] [isPOD=0]
// CHECK32: StructDecl=C:[[@LINE+1]]:8 (Definition) typekind=Record [size=64] [alignment=32] [isPOD=0]
struct C {
// Make fields private so C won't be a POD type.
private:
  int a;
  char b;
};

// CHECK64: StructDecl=D:[[@LINE+2]]:8 (Definition) typekind=Record [size=64] [alignment=32] [isPOD=0]
// CHECK32: StructDecl=D:[[@LINE+1]]:8 (Definition) typekind=Record [size=64] [alignment=32] [isPOD=0]
struct D : C {
  char c;
};

// CHECK64: StructDecl=E:[[@LINE+2]]:32 (Definition) typekind=Record [size=40] [alignment=8] [isPOD=1]
// CHECK32: StructDecl=E:[[@LINE+1]]:32 (Definition) typekind=Record [size=40] [alignment=8] [isPOD=1]
struct __attribute__((packed)) E {
  char b;
  int a;
};

// CHECK64: StructDecl=F:[[@LINE+2]]:32 (Definition) typekind=Record [size=48] [alignment=8] [isPOD=0]
// CHECK32: StructDecl=F:[[@LINE+1]]:32 (Definition) typekind=Record [size=48] [alignment=8] [isPOD=0]
struct __attribute__((packed)) F : E {
  char d;
};

struct G { G(); };
// CHECK64: StructDecl=H:[[@LINE+2]]:8 (Definition) typekind=Record [size=8] [alignment=8] [isPOD=0]
// CHECK32: StructDecl=H:[[@LINE+1]]:8 (Definition) typekind=Record [size=8] [alignment=8] [isPOD=0]
struct H : G { };

// CHECK64: StructDecl=I:[[@LINE+2]]:8 (Definition) typekind=Record [size=40] [alignment=8] [isPOD=1]
// CHECK32: StructDecl=I:[[@LINE+1]]:8 (Definition) typekind=Record [size=40] [alignment=8] [isPOD=1]
struct I {
  char b;
  int a;
} __attribute__((packed));

// PR5580
namespace PR5580 {

// CHECK64: ClassDecl=A:[[@LINE+2]]:7 (Definition) typekind=Record [size=8] [alignment=8] [isPOD=0]
// CHECK32: ClassDecl=A:[[@LINE+1]]:7 (Definition) typekind=Record [size=8] [alignment=8] [isPOD=0]
class A { bool iv0 : 1; };

// CHECK64: ClassDecl=B:[[@LINE+2]]:7 (Definition) typekind=Record [size=16] [alignment=8] [isPOD=0]
// CHECK32: ClassDecl=B:[[@LINE+1]]:7 (Definition) typekind=Record [size=16] [alignment=8] [isPOD=0]
class B : A { bool iv0 : 1; };

// CHECK64: StructDecl=C:[[@LINE+2]]:8 (Definition) typekind=Record [size=8] [alignment=8] [isPOD=1]
// CHECK32: StructDecl=C:[[@LINE+1]]:8 (Definition) typekind=Record [size=8] [alignment=8] [isPOD=1]
struct C { bool iv0 : 1; };

// CHECK64: StructDecl=D:[[@LINE+2]]:8 (Definition) typekind=Record [size=16] [alignment=8] [isPOD=0]
// CHECK32: StructDecl=D:[[@LINE+1]]:8 (Definition) typekind=Record [size=16] [alignment=8] [isPOD=0]
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
// CHECK64: StructDecl=G:[[@LINE+2]]:8 (Definition) typekind=Record [size=192] [alignment=64] [isPOD=0]
// CHECK32: StructDecl=G:[[@LINE+1]]:8 (Definition) typekind=Record [size=128] [alignment=32] [isPOD=0]
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
// CHECK64: StructDecl=H:[[@LINE+2]]:8 (Definition) typekind=Record [size=192] [alignment=64] [isPOD=0]
// CHECK32: StructDecl=H:[[@LINE+1]]:8 (Definition) typekind=Record [size=96] [alignment=32] [isPOD=0]
struct H { G g; };

}

namespace Test3 {


// CHECK64: ClassDecl=B:[[@LINE+2]]:7 (Definition) typekind=Record [size=128] [alignment=64] [isPOD=0]
// CHECK32: ClassDecl=B:[[@LINE+1]]:7 (Definition) typekind=Record [size=64] [alignment=32] [isPOD=0]
class B {
public:
  virtual void b(){}
// CHECK64: FieldDecl=b_field:[[@LINE+2]]:8 (Definition) typekind=Long [offset=64] [isPOD=1]
// CHECK32: FieldDecl=b_field:[[@LINE+1]]:8 (Definition) typekind=Long [offset=32] [isPOD=1]
  long b_field;
protected:
private:
};

// CHECK32: ClassDecl=A:[[@LINE+1]]:7 (Definition) typekind=Record [size=128] [alignment=32] [isPOD=0]
class A : public B {
public:
// CHECK64: FieldDecl=a_field:[[@LINE+2]]:7 (Definition) typekind=Int [offset=128] [isPOD=1]
// CHECK32: FieldDecl=a_field:[[@LINE+1]]:7 (Definition) typekind=Int [offset=64] [isPOD=1]
  int a_field;
  virtual void a(){}
// CHECK64: FieldDecl=one:[[@LINE+2]]:8 (Definition) typekind=Char_S [offset=160] [isPOD=1]
// CHECK32: FieldDecl=one:[[@LINE+1]]:8 (Definition) typekind=Char_S [offset=96] [isPOD=1]
  char one;
protected:
private:
};

// CHECK64: ClassDecl=D:[[@LINE+2]]:7 (Definition) typekind=Record [size=128] [alignment=64] [isPOD=0]
// CHECK32: ClassDecl=D:[[@LINE+1]]:7 (Definition) typekind=Record [size=96] [alignment=32] [isPOD=0]
class D {
public:
  virtual void b(){}
// CHECK64: FieldDecl=a:[[@LINE+2]]:10 (Definition) typekind=Double [offset=64] [isPOD=1]
// CHECK32: FieldDecl=a:[[@LINE+1]]:10 (Definition) typekind=Double [offset=32] [isPOD=1]
  double a;
};

// CHECK64: ClassDecl=C:[[@LINE+2]]:7 (Definition) typekind=Record [size=704] [alignment=64] [isPOD=0]
// CHECK32: ClassDecl=C:[[@LINE+1]]:7 (Definition) typekind=Record [size=480] [alignment=32] [isPOD=0]
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
    C &rg;
    int x;
};

struct DerivedStruct : public BaseStruct { int x; };
struct G { int g_field; };
struct H : public G, public virtual D {};

// CHECK64: StructDecl=I:[[@LINE+2]]:8 (Definition) typekind=Record [size=256] [alignment=64] [isPOD=0]
// CHECK32: StructDecl=I:[[@LINE+1]]:8 (Definition) typekind=Record [size=192] [alignment=32] [isPOD=0]
struct I : public virtual D
{
  virtual ~I(){}
  double q;
};

struct K { int k; };

struct L { int l; };

struct M : public virtual K { int m; };

// CHECK64: StructDecl=N:[[@LINE+2]]:8 (Definition) typekind=Record [size=192] [alignment=64] [isPOD=0]
// CHECK32: StructDecl=N:[[@LINE+1]]:8 (Definition) typekind=Record [size=128] [alignment=32] [isPOD=0]
struct N : public L, public M { virtual void f(){} };

// CHECK64: StructDecl=O:[[@LINE+2]]:8 (Definition) typekind=Record [size=256] [alignment=64] [isPOD=0]
// CHECK32: StructDecl=O:[[@LINE+1]]:8 (Definition) typekind=Record [size=192] [alignment=32] [isPOD=0]
struct O : public H, public G { virtual void fo(){} };

// CHECK64: StructDecl=P:[[@LINE+2]]:8 (Definition) typekind=Record [size=192] [alignment=64] [isPOD=0]
// CHECK32: StructDecl=P:[[@LINE+1]]:8 (Definition) typekind=Record [size=160] [alignment=32] [isPOD=0]
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

// CHECK64: StructDecl=sd:[[@LINE+2]]:8 (Definition) typekind=Record [size=256] [alignment=64] [isPOD=0]
// CHECK32: StructDecl=sd:[[@LINE+1]]:8 (Definition) typekind=Record [size=160] [alignment=32] [isPOD=0]
struct sd : virtual s, virtual ICh {
  virtual ~sd(){}
// CHECK64: FieldDecl=q:[[@LINE+2]]:7 (Definition) typekind=Int [offset=64] [isPOD=1]
// CHECK32: FieldDecl=q:[[@LINE+1]]:7 (Definition) typekind=Int [offset=32] [isPOD=1]
  int q;
// CHECK64: FieldDecl=y:[[@LINE+2]]:8 (Definition) typekind=Char_S [offset=96] [isPOD=1]
// CHECK32: FieldDecl=y:[[@LINE+1]]:8 (Definition) typekind=Char_S [offset=64] [isPOD=1]
  char y;
  virtual int asd() {return -1;}
};

// CHECK64: StructDecl=AV:[[@LINE+2]]:8 (Definition) typekind=Record [size=64] [alignment=64] [isPOD=0]
// CHECK32: StructDecl=AV:[[@LINE+1]]:8 (Definition) typekind=Record [size=32] [alignment=32] [isPOD=0]
struct AV { virtual void foo(); };
// CHECK64: StructDecl=BV:[[@LINE+2]]:8 (Definition) typekind=Record [size=64] [alignment=64] [isPOD=0]
// CHECK32: StructDecl=BV:[[@LINE+1]]:8 (Definition) typekind=Record [size=32] [alignment=32] [isPOD=0]
struct BV : AV {};
// CHECK64: StructDecl=CV:[[@LINE+2]]:8 (Definition) typekind=Record [size=64] [alignment=64] [isPOD=0]
// CHECK32: StructDecl=CV:[[@LINE+1]]:8 (Definition) typekind=Record [size=32] [alignment=32] [isPOD=0]
struct CV : virtual BV {
  CV();
  virtual void foo();
};
// CHECK64: StructDecl=DV:[[@LINE+2]]:8 (Definition) typekind=Record [size=64] [alignment=64] [isPOD=0]
// CHECK32: StructDecl=DV:[[@LINE+1]]:8 (Definition) typekind=Record [size=32] [alignment=32] [isPOD=0]
struct DV : BV {};
// CHECK64: StructDecl=EV:[[@LINE+2]]:8 (Definition) typekind=Record [size=128] [alignment=64] [isPOD=0]
// CHECK32: StructDecl=EV:[[@LINE+1]]:8 (Definition) typekind=Record [size=64] [alignment=32] [isPOD=0]
struct EV : CV, DV {};

}


// RUN: c-index-test -test-print-typekind %s -target i386-linux-gnu | FileCheck -check-prefix=CHECK1 %s
// RUN: c-index-test -test-print-typekind %s -target nvptx64-unknown-unknown | FileCheck -check-prefix=CHECK2 %s
// RUN: c-index-test -test-print-typekind %s -target i386-pc-win32 | FileCheck -check-prefix=CHECK3 %s
// RUN: c-index-test -test-print-typekind %s -target msp430-none-none | FileCheck -check-prefix=CHECK4 %s

// CHECK1: StructDecl=dalign:[[@LINE+4]]:8 (Definition) typekind=Record [size=128] [alignment=32] [isPOD=1]
// CHECK2: StructDecl=dalign:[[@LINE+3]]:8 (Definition) typekind=Record [size=192] [alignment=64] [isPOD=1]
// CHECK3: StructDecl=dalign:[[@LINE+2]]:8 (Definition) typekind=Record [size=128] [alignment=64] [isPOD=1]
// CHECK4: StructDecl=dalign:[[@LINE+1]]:8 (Definition) typekind=Record [size=128] [alignment=64] [isPOD=1]
struct dalign {
// CHECK1: FieldDecl=a1:[[@LINE+4]]:10 (Definition) typekind=Long [offset=0] [isPOD=1]
// CHECK2: FieldDecl=a1:[[@LINE+3]]:10 (Definition) typekind=Long [offset=0] [isPOD=1]
// CHECK3: FieldDecl=a1:[[@LINE+2]]:10 (Definition) typekind=Long [offset=0] [isPOD=1]
// CHECK4: FieldDecl=a1:[[@LINE+1]]:10 (Definition) typekind=Long [offset=0] [isPOD=1]
    long a1;
// CHECK1: FieldDecl=a2:[[@LINE+4]]:10 (Definition) typekind=Long [offset=32] [isPOD=1]
// CHECK2: FieldDecl=a2:[[@LINE+3]]:10 (Definition) typekind=Long [offset=64] [isPOD=1]
// CHECK3: FieldDecl=a2:[[@LINE+2]]:10 (Definition) typekind=Long [offset=32] [isPOD=1]
// CHECK4: FieldDecl=a2:[[@LINE+1]]:10 (Definition) typekind=Long [offset=32] [isPOD=1]
    long a2:3;
// CHECK1: FieldDecl=a3:[[@LINE+4]]:10 (Definition) typekind=Long [offset=35] [isPOD=1]
// CHECK2: FieldDecl=a3:[[@LINE+3]]:10 (Definition) typekind=Long [offset=67] [isPOD=1]
// CHECK3: FieldDecl=a3:[[@LINE+2]]:10 (Definition) typekind=Long [offset=35] [isPOD=1]
// CHECK4: FieldDecl=a3:[[@LINE+1]]:10 (Definition) typekind=Long [offset=35] [isPOD=1]
    long a3:4;
// CHECK1: FieldDecl=a4:[[@LINE+4]]:12 (Definition) typekind=Double [offset=64] [isPOD=1]
// CHECK2: FieldDecl=a4:[[@LINE+3]]:12 (Definition) typekind=Double [offset=128] [isPOD=1]
// CHECK3: FieldDecl=a4:[[@LINE+2]]:12 (Definition) typekind=Double [offset=64] [isPOD=1]
// CHECK4: FieldDecl=a4:[[@LINE+1]]:12 (Definition) typekind=Double [offset=64] [isPOD=1]
    double a4;
};

// CHECK1: StructDecl=salign:[[@LINE+4]]:8 (Definition) typekind=Record [size=96] [alignment=32] [isPOD=1]
// CHECK2: StructDecl=salign:[[@LINE+3]]:8 (Definition) typekind=Record [size=128] [alignment=64] [isPOD=1]
// CHECK3: StructDecl=salign:[[@LINE+2]]:8 (Definition) typekind=Record [size=96] [alignment=32] [isPOD=1]
// CHECK4: StructDecl=salign:[[@LINE+1]]:8 (Definition) typekind=Record [size=64] [alignment=16] [isPOD=1]
struct salign {
    short b1;    
    int b2;    
    long b3;    
};

namespace Test4 {
class A;
class B {
  A* a1;
  A& a2;
};
}

