struct s {
  int a;
  char b;
  long long c;
  int d:3;
  int e:4;
  int f:4;
};
union u {
  int u1;
  long long u2;
  struct s s1;
};

// RUN: c-index-test -test-print-typekind %s -m64 | FileCheck -check-prefix=CHECK64 %s
// CHECK64: StructDecl=s:1:8 (Definition) typekind=Record [size=24] [alignment=8] [isPOD=1]
// CHECK64: FieldDecl=a:2:7 (Definition) typekind=Int [offset=0] [isPOD=1]
// CHECK64: FieldDecl=b:3:8 (Definition) typekind=Char_S [offset=32] [isPOD=1]
// CHECK64: FieldDecl=c:4:13 (Definition) typekind=LongLong [offset=64] [isPOD=1]
// CHECK64: FieldDecl=d:5:7 (Definition) typekind=Int [offset=128] [isPOD=1]
// CHECK64: IntegerLiteral= typekind=Int [isPOD=1]
// CHECK64: FieldDecl=e:6:7 (Definition) typekind=Int [offset=131] [isPOD=1]
// CHECK64: IntegerLiteral= typekind=Int [isPOD=1]
// CHECK64: FieldDecl=f:7:7 (Definition) typekind=Int [offset=135] [isPOD=1]
// CHECK64: IntegerLiteral= typekind=Int [isPOD=1]
// CHECK64: UnionDecl=u:9:7 (Definition) typekind=Record [size=24] [alignment=8] [isPOD=1]
// CHECK64: FieldDecl=u1:10:7 (Definition) typekind=Int [offset=0] [isPOD=1]
// CHECK64: FieldDecl=u2:11:13 (Definition) typekind=LongLong [offset=0] [isPOD=1]
// CHECK64: FieldDecl=s1:12:12 (Definition) typekind=Unexposed [canonical=Record] [size=24] [alignment=8] [offset=0] [isPOD=1]
// CHECK64: TypeRef=struct s:1:8 typekind=Record [size=24] [alignment=8] [isPOD=1]

// RUN: c-index-test -test-print-typekind %s -m32 | FileCheck -check-prefix=CHECK32 %s
// CHECK32: StructDecl=s:1:8 (Definition) typekind=Record [size=20] [alignment=4] [isPOD=1]
// CHECK32: FieldDecl=a:2:7 (Definition) typekind=Int [offset=0] [isPOD=1]
// CHECK32: FieldDecl=b:3:8 (Definition) typekind=Char_S [offset=32] [isPOD=1]
// CHECK32: FieldDecl=c:4:13 (Definition) typekind=LongLong [offset=64] [isPOD=1]
// CHECK32: FieldDecl=d:5:7 (Definition) typekind=Int [offset=128] [isPOD=1]
// CHECK32: IntegerLiteral= typekind=Int [isPOD=1]
// CHECK32: FieldDecl=e:6:7 (Definition) typekind=Int [offset=131] [isPOD=1]
// CHECK32: IntegerLiteral= typekind=Int [isPOD=1]
// CHECK32: FieldDecl=f:7:7 (Definition) typekind=Int [offset=135] [isPOD=1]
// CHECK32: IntegerLiteral= typekind=Int [isPOD=1]
// CHECK32: UnionDecl=u:9:7 (Definition) typekind=Record [size=20] [alignment=4] [isPOD=1]
// CHECK32: FieldDecl=u1:10:7 (Definition) typekind=Int [offset=0] [isPOD=1]
// CHECK32: FieldDecl=u2:11:13 (Definition) typekind=LongLong [offset=0] [isPOD=1]
// CHECK32: FieldDecl=s1:12:12 (Definition) typekind=Unexposed [canonical=Record] [size=20] [alignment=4] [offset=0] [isPOD=1]
// CHECK32: TypeRef=struct s:1:8 typekind=Record [size=20] [alignment=4] [isPOD=1]

