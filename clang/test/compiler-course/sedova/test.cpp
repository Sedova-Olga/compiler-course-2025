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
// CHECK: BinaryOperator {{.*}} 'double' '+'
// CHECK: ImplicitCastExpr {{.*}} 'float' <IntegralToFloating>
// CHECK: 'int' lvalue ParmVar
// CHECK: ImplicitCastExpr {{.*}} 'double' <FloatingCast>
// CHECK: 'float' lvalue ParmVar

double compute(int x, float y) {
  return x + y;
}

// CHECK-LABEL: FunctionDecl {{.*}} process 'int (float, float)'
// CHECK: ParmVarDecl {{.*}} 'float'
// CHECK: ParmVarDecl {{.*}} 'float'
// CHECK: ReturnStmt
// CHECK: ImplicitCastExpr {{.*}} 'int' <FloatingToIntegral>
// CHECK: BinaryOperator {{.*}} 'double' '+'
// CHECK: ImplicitCastExpr {{.*}} 'double' <FloatingCast>
// CHECK: 'float' lvalue ParmVar
// CHECK: CallExpr
// CHECK: ImplicitCastExpr {{.*}} <FunctionToPointerDecay>
// CHECK: 'double (int, float)' lvalue Function
// CHECK: ImplicitCastExpr {{.*}} 'int' <FloatingToIntegral>
// CHECK: 'float' lvalue ParmVar

int process(float a, float b) {
  return a + compute(a, b);
}

// CHECK-LABEL: FunctionDecl {{.*}} identity 'int (int)'
// CHECK: ParmVarDecl {{.*}} 'int'
// CHECK: ReturnStmt
// CHECK: ImplicitCastExpr {{.*}} 'int' <LValueToRValue>
// CHECK: 'int' lvalue ParmVar

int identity(int v) {
  return v;
}

// CHECK-LABEL: FunctionDecl {{.*}} multipleCasts 'double (int, int)'
// CHECK: ParmVarDecl {{.*}} 'int'
// CHECK: ParmVarDecl {{.*}} 'int'
// CHECK: VarDecl {{.*}} 'float'
// CHECK: BinaryOperator {{.*}} 'int' '+'
// CHECK: 'int' lvalue ParmVar
// CHECK: ImplicitCastExpr {{.*}} 'float' <IntegralToFloating>
// CHECK: ReturnStmt
// CHECK: BinaryOperator {{.*}} 'double' '+'
// CHECK: ImplicitCastExpr {{.*}} 'double' <FloatingCast>
// CHECK: 'float' lvalue Var
// CHECK: FloatingLiteral {{.*}} 'double'

double multipleCasts(int a, int b) {
  float fa = a + b;
  return fa + 1.0;
}

// CHECK-LABEL: FunctionDecl {{.*}} explicitCast 'float (int)'
// CHECK: ParmVarDecl {{.*}} 'int'
// CHECK: CStyleCastExpr {{.*}} 'float'
// CHECK: 'int' lvalue ParmVar

float explicitCast(int a) {
  return (float)a;
}

// CHECK-LABEL: FunctionDecl {{.*}} returnFloat 'float (float)'
// CHECK: ParmVarDecl {{.*}} 'float'
// CHECK: ReturnStmt
// CHECK: ImplicitCastExpr {{.*}} 'float' <LValueToRValue>
// CHECK: 'float' lvalue ParmVar

float returnFloat(float f) {
  return f;
}