// RUN: %clang_cc1 -load %llvmshlibdir/Lab1_Sedova_Olga_FIIT1_ClangAST%pluginext -plugin Lab1 -fsyntax-only %s 2>&1 | FileCheck %s

// CHECK: FunctionDecl {{.*}} isPositive 'bool (int)'
// CHECK-NEXT: |-ParmVarDecl {{.*}} a 'int'
// CHECK-NEXT: `-CompoundStmt

bool isPositive(int a) {
  return a > 0;
}

// CHECK: FunctionDecl {{.*}} compute 'double (int, float)'
// CHECK-NEXT: |-ParmVarDecl {{.*}} x 'int'
// CHECK-NEXT: |-ParmVarDecl {{.*}} y 'float'
// CHECK-NEXT: `-CompoundStmt
// CHECK: ReturnStmt
// CHECK: BinaryOperator {{.*}} 'double' '+'
// CHECK: ImplicitCastExpr {{.*}} <IntegralToFloating>
// CHECK: DeclRefExpr {{.*}} 'int' lvalue ParmVar {{.*}} 'x' 'int'
// CHECK: ImplicitCastExpr {{.*}} <FloatingCast>
// CHECK: DeclRefExpr {{.*}} 'float' lvalue ParmVar {{.*}} 'y' 'float'

double compute(int x, float y) {
  return x + y;
}

// CHECK: FunctionDecl {{.*}} process 'int (float, float)'
// CHECK-NEXT: |-ParmVarDecl {{.*}} a 'float'
// CHECK-NEXT: |-ParmVarDecl {{.*}} b 'float'
// CHECK-NEXT: `-CompoundStmt
// CHECK: ReturnStmt
// CHECK: ImplicitCastExpr {{.*}} <FloatingToIntegral>
// CHECK: BinaryOperator {{.*}} 'double' '+'
// CHECK: ImplicitCastExpr {{.*}} <FloatingCast>
// CHECK: DeclRefExpr {{.*}} 'float' lvalue ParmVar {{.*}} 'a' 'float'
// CHECK: CallExpr
// CHECK: ImplicitCastExpr {{.*}} <FunctionToPointerDecay>
// CHECK: DeclRefExpr {{.*}} 'double (int, float)' lvalue Function {{.*}} 'compute'
// CHECK: ImplicitCastExpr {{.*}} <FloatingToIntegral>
// CHECK: ImplicitCastExpr {{.*}} <LValueToRValue>
// CHECK: DeclRefExpr {{.*}} 'float' lvalue ParmVar {{.*}} 'a' 'float'
// CHECK: ImplicitCastExpr {{.*}} <LValueToRValue>
// CHECK: DeclRefExpr {{.*}} 'float' lvalue ParmVar {{.*}} 'b' 'float'

int process(float a, float b) {
  return a + compute(a, b);
}

// CHECK: FunctionDecl {{.*}} identity 'int (int)'
// CHECK-NEXT: |-ParmVarDecl {{.*}} v 'int'
// CHECK-NEXT: `-CompoundStmt
// CHECK: ReturnStmt
// CHECK: ImplicitCastExpr {{.*}} <LValueToRValue>
// CHECK: DeclRefExpr {{.*}} 'int' lvalue ParmVar {{.*}} 'v' 'int'

int identity(int v) {
  return v;
}

// CHECK: FunctionDecl {{.*}} multipleCasts 'double (int, int)'
// CHECK-NEXT: |-ParmVarDecl {{.*}} a 'int'
// CHECK-NEXT: |-ParmVarDecl {{.*}} b 'int'
// CHECK-NEXT: `-CompoundStmt
// CHECK: VarDecl {{.*}} fa 'float'
// CHECK: BinaryOperator {{.*}} 'float' '+'
// CHECK: ImplicitCastExpr {{.*}} <IntegralToFloating>
// CHECK: DeclRefExpr {{.*}} 'int' lvalue ParmVar {{.*}} 'a' 'int'
// CHECK: ImplicitCastExpr {{.*}} <IntegralToFloating>
// CHECK: DeclRefExpr {{.*}} 'int' lvalue ParmVar {{.*}} 'b' 'int'
// CHECK: ReturnStmt
// CHECK: BinaryOperator {{.*}} 'double' '+'
// CHECK: ImplicitCastExpr {{.*}} <FloatingCast>
// CHECK: ImplicitCastExpr {{.*}} <LValueToRValue>
// CHECK: DeclRefExpr {{.*}} 'float' lvalue Var {{.*}} 'fa' 'float'
// CHECK: FloatingLiteral {{.*}} 'double'

double multipleCasts(int a, int b) {
  float fa = a + b;
  return fa + 1.0;
}

// CHECK: FunctionDecl {{.*}} explicitCast 'float (int)'
// CHECK-NEXT: |-ParmVarDecl {{.*}} a 'int'
// CHECK-NEXT: `-CompoundStmt
// CHECK: ReturnStmt
// CHECK: CStyleCastExpr {{.*}} 'float' <CastKind::CK_IntegralToFloating>
// CHECK: DeclRefExpr {{.*}} 'int' lvalue ParmVar {{.*}} 'a' 'int'

float explicitCast(int a) {
  return (float)a;
}

// CHECK: FunctionDecl {{.*}} returnFloat 'float (float)'
// CHECK-NEXT: |-ParmVarDecl {{.*}} f 'float'
// CHECK-NEXT: `-CompoundStmt
// CHECK: ReturnStmt
// CHECK: ImplicitCastExpr {{.*}} <LValueToRValue>
// CHECK: DeclRefExpr {{.*}} 'float' lvalue ParmVar {{.*}} 'f' 'float'

float returnFloat(float f) {
  return f;
}
