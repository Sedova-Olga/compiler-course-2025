// RUN: %clang_cc1 -load %llvmshlibdir/Lab1_Sedova_Olga_FIIT1_ClangAST%pluginext -plugin Lab1 -fsyntax-only %s 2>&1 | FileCheck %s

// CHECK-LABEL: FunctionDecl {{.*}} isPositive 'bool (int)'
// CHECK: ParmVarDecl {{.*}} 'int'
// CHECK: BinaryOperator {{.*}} 'bool' '>'

bool isPositive(int a) {
  return a > 0;
}

// CHECK-LABEL: FunctionDecl {{.*}} compute 'double (int, float)'
// CHECK: ParmVarDecl {{.*}} 'int'
// CHECK: ParmVarDecl {{.*}} 'float'
// CHECK: ReturnStmt
// CHECK: ImplicitCastExpr {{.*}} 'double' <FloatingCast>
// CHECK: BinaryOperator {{.*}} 'float' '+'
// CHECK: ImplicitCastExpr {{.*}} 'float' <IntegralToFloating>
// CHECK: 'int' lvalue ParmVar
// CHECK: ImplicitCastExpr {{.*}} 'float' <LValueToRValue>
// CHECK: 'float' lvalue ParmVar

double compute(int x, float y) {
  return x + y;
}

// CHECK-LABEL: FunctionDecl {{.*}} multipleCasts 'double (int, int)'
// CHECK: ParmVarDecl {{.*}} 'int'
// CHECK: ParmVarDecl {{.*}} 'int'
// CHECK: VarDecl {{.*}} 'float'
// CHECK: ImplicitCastExpr {{.*}} 'float' <IntegralToFloating>
// CHECK: BinaryOperator {{.*}} 'int' '+'
// CHECK: 'int' lvalue ParmVar
// CHECK: 'int' lvalue ParmVar
// CHECK: ReturnStmt
// CHECK: BinaryOperator {{.*}} 'double' '+'
// CHECK: ImplicitCastExpr {{.*}} 'double' <FloatingCast>
// CHECK: ImplicitCastExpr {{.*}} 'float' <LValueToRValue>
// CHECK: 'float' lvalue Var
// CHECK: FloatingLiteral {{.*}} 'double'

double multipleCasts(int a, int b) {
  float fa = a + b;
  return fa + 1.0;
}