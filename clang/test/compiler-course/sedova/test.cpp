// RUN: %clang_cc1 -load %llvmshlibdir/Lab1_Sedova_Olga_FIIT1_ClangAST%pluginext -plugin Lab1 -fsyntax-only %s 2>&1 | FileCheck %s

// CHECK: FunctionDecl {{.*}} isPositive 'bool (int)'
// CHECK-NEXT: |-ParmVarDecl {{.*}} a 'int'
// CHECK-NEXT: `-CompoundStmt

bool isPositive(int a) {
  return a > 0;
}
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
// CHECK-NEXT:   `-ReturnStmt
// CHECK-NEXT:     `-BinaryOperator {{.*}} 'double' '+'
// CHECK-NEXT:       |-ImplicitCastExpr {{.*}} 'float' <IntegralToFloating>
// CHECK-NEXT:       | `-DeclRefExpr {{.*}} 'int' lvalue ParmVar {{.*}} 'x' 'int'
// CHECK-NEXT:       `-ImplicitCastExpr {{.*}} 'double' <FloatingCast>
// CHECK-NEXT:         `-DeclRefExpr {{.*}} 'float' lvalue ParmVar {{.*}} 'y' 'float'

double compute(int x, float y) {
  return x + y;
}

// CHECK: FunctionDecl {{.*}} process 'int (float, float)'
// CHECK-NEXT: |-ParmVarDecl {{.*}} a 'float'
// CHECK-NEXT: |-ParmVarDecl {{.*}} b 'float'
// CHECK-NEXT: `-CompoundStmt
// CHECK-NEXT:   `-ReturnStmt
// CHECK-NEXT:     `-ImplicitCastExpr {{.*}} 'int' <FloatingToIntegral>
// CHECK-NEXT:       `-BinaryOperator {{.*}} 'double' '+'
// CHECK-NEXT:         |-ImplicitCastExpr {{.*}} 'double' <FloatingCast>
// CHECK-NEXT:         | `-DeclRefExpr {{.*}} 'float' lvalue ParmVar {{.*}} 'a' 'float'
// CHECK-NEXT:         `-CallExpr {{.*}} 'double'
// CHECK-NEXT:           |-ImplicitCastExpr {{.*}} 'double (*)(int, float)' <FunctionToPointerDecay>
// CHECK-NEXT:           | `-DeclRefExpr {{.*}} 'double (int, float)' lvalue Function {{.*}} 'compute' 'double (int, float)'
// CHECK-NEXT:           |-ImplicitCastExpr {{.*}} 'int' <FloatingToIntegral>
// CHECK-NEXT:           | `-ImplicitCastExpr {{.*}} 'float' <LValueToRValue>
// CHECK-NEXT:           |   `-DeclRefExpr {{.*}} 'float' lvalue ParmVar {{.*}} 'a' 'float'
// CHECK-NEXT:           `-ImplicitCastExpr {{.*}} 'float' <LValueToRValue>
// CHECK-NEXT:             `-DeclRefExpr {{.*}} 'float' lvalue ParmVar {{.*}} 'b' 'float'

int process(float a, float b) {
  return a + compute(a, b);
}

// CHECK: FunctionDecl {{.*}} identity 'int (int)'
// CHECK-NEXT: |-ParmVarDecl {{.*}} v 'int'
// CHECK-NEXT: `-CompoundStmt
// CHECK-NEXT:   `-ReturnStmt
// CHECK-NEXT:     `-ImplicitCastExpr {{.*}} 'int' <LValueToRValue>
// CHECK-NEXT:       `-DeclRefExpr {{.*}} 'int' lvalue ParmVar {{.*}} 'v' 'int'

int identity(int v) {
  return v;
}

// CHECK: FunctionDecl {{.*}} multipleCasts 'double (int, int)'
// CHECK-NEXT: |-ParmVarDecl {{.*}} a 'int'
// CHECK-NEXT: |-ParmVarDecl {{.*}} b 'int'
// CHECK-NEXT: `-CompoundStmt
// CHECK-NEXT:   |-DeclStmt
// CHECK-NEXT:   | `-VarDecl {{.*}} fa 'float'
// CHECK-NEXT:   |   `-BinaryOperator {{.*}} 'float' '+'
// CHECK-NEXT:   |     |-ImplicitCastExpr {{.*}} 'float' <IntegralToFloating>
// CHECK-NEXT:   |     | `-DeclRefExpr {{.*}} 'int' lvalue ParmVar {{.*}} 'a' 'int'
// CHECK-NEXT:   |     `-ImplicitCastExpr {{.*}} 'float' <IntegralToFloating>
// CHECK-NEXT:   |       `-DeclRefExpr {{.*}} 'int' lvalue ParmVar {{.*}} 'b' 'int'
// CHECK-NEXT:   `-ReturnStmt
// CHECK-NEXT:     `-BinaryOperator {{.*}} 'double' '+'
// CHECK-NEXT:       |-ImplicitCastExpr {{.*}} 'double' <FloatingCast>
// CHECK-NEXT:       | `-ImplicitCastExpr {{.*}} 'float' <LValueToRValue>
// CHECK-NEXT:       |   `-DeclRefExpr {{.*}} 'float' lvalue Var {{.*}} 'fa' 'float'
// CHECK-NEXT:       `-FloatingLiteral {{.*}} 'double'

double multipleCasts(int a, int b) {
  float fa = a + b;
  return fa + 1.0;
}

// CHECK: FunctionDecl {{.*}} explicitCast 'float (int)'
// CHECK-NEXT: |-ParmVarDecl {{.*}} a 'int'
// CHECK-NEXT: `-CompoundStmt
// CHECK-NEXT:   `-ReturnStmt
// CHECK-NEXT:     `-CStyleCastExpr {{.*}} 'float' <CastKind::CK_IntegralToFloating>
// CHECK-NEXT:       `-DeclRefExpr {{.*}} 'int' lvalue ParmVar {{.*}} 'a' 'int'

float explicitCast(int a) {
  return (float)a;
}

// CHECK: FunctionDecl {{.*}} returnFloat 'float (float)'
// CHECK-NEXT: |-ParmVarDecl {{.*}} f 'float'
// CHECK-NEXT: `-CompoundStmt
// CHECK-NEXT:   `-ReturnStmt
// CHECK-NEXT:     `-ImplicitCastExpr {{.*}} 'float' <LValueToRValue>
// CHECK-NEXT:       `-DeclRefExpr {{.*}} 'float' lvalue ParmVar {{.*}} 'f' 'float'

float returnFloat(float f) {
  return f;
}
