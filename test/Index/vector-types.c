int __attribute__((vector_size(16))) x;
typedef int __attribute__((vector_size(16))) int4_t;

// RUN: c-index-test -test-print-typekind %s -target i386-linux-gnu | FileCheck %s
// CHECK: VarDecl=x:1:38 typekind=Vector [isPOD=1]
// CHECK: TypedefDecl=int4_t:2:46 (Definition) typekind=Typedef [canonical=Vector] [size=128] [alignment=128] [isPOD=1]
