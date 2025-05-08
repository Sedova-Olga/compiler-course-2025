// RUN: %clang_cc1 -load %llvmshlibdir/Lab1_Sedova_Olga_FIIT1_ClangAST%pluginext -plugin Lab1 -fsyntax-only %s 2>&1 | FileCheck %s

// CHECK: FunctionDecl {{.*}} isPositive 'bool (int)'
// CHECK-NEXT: |-ParmVarDecl {{.*}} a 'int'
// CHECK-NEXT: `-CompoundStmt

bool isPositive(int a) {
  return a > 0;
}

// CHECK-LABEL: Function `compute`
// CHECK: int -> float: 1
// CHECK: float -> double: 1
double compute(int x, float y) {
  return x + y;
}

// CHECK-LABEL: Function `process`
// CHECK: float -> int: 1
// CHECK: float -> double: 1
// CHECK: double -> int: 1
int process(float a, float b) {
  return a + compute(a, b);
}

// CHECK-LABEL: Function `identity`
// CHECK-NOT: ->
int identity(int v) {
  return v;
}

// CHECK-LABEL: Function `multipleCasts`
// CHECK: int -> float: 2
// CHECK: float -> double: 2
double multipleCasts(int a, int b) {
  float fa = a + b;
  return fa + 1.0;
}

// CHECK-LABEL: Function `explicitCast`
// CHECK-NOT: int -> float
float explicitCast(int a) {
  return (float)a;
}

// CHECK-LABEL: Function `returnFloat`
// CHECK-NOT: ->
float returnFloat(float f) {
  return f;
}