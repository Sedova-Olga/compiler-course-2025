// RUN: %clang_cc1 -load %llvmshlibdir/Lab1_Sedova_Olga_FIIT1_ClangAST%pluginext -plugin Lab1 -fsyntax-only %s 2>&1 | FileCheck %s

// Simple function
// CHECK: FunctionDecl {{0x[0-9a-fA-F]+}} <{{.*}}> col:1 isPositive 'bool (int)'
// CHECK-NEXT: |-ParmVarDecl {{0x[0-9a-fA-F]+}} <col:18, col:22> col:22 used x 'int'
// CHECK-NEXT: |-CompoundStmt {{0x[0-9a-fA-F]+}} <col:25, line:3:1>
// CHECK-NEXT: | `-ReturnStmt {{0x[0-9a-fA-F]+}} <line:2:3, col:14>
// CHECK-NEXT: |   `-BinaryOperator {{0x[0-9a-fA-F]+}} <col:10, col:14> 'bool' '>'
// CHECK-NEXT: |     |-ImplicitCastExpr {{0x[0-9a-fA-F]+}} <col:10> 'int' <LValueToRValue>
// CHECK-NEXT: |     | `-DeclRefExpr {{0x[0-9a-fA-F]+}} <col:10> 'int' lvalue ParmVar {{0x[0-9a-fA-F]+}} 'x' 'int'
// CHECK-NEXT: |     `-IntegerLiteral {{0x[0-9a-fA-F]+}} <col:14> 'int' 0
bool isPositive(int x) {
  return x > 0;
}

// CHECK: FunctionDecl {{0x[0-9a-fA-F]+}} <line:6:1, line:8:1> line:6:5 factorial 'int (int)'
// CHECK-NEXT: |-ParmVarDecl {{0x[0-9a-fA-F]+}} <col:15, col:19> col:19 used n 'int'
// CHECK-NEXT: |-CompoundStmt {{0x[0-9a-fA-F]+}} <col:22, line:8:1>
// CHECK-NEXT: | `-IfStmt {{0x[0-9a-fA-F]+}} <line:7:3, line:8:1>
// CHECK-NEXT: |   |-BinaryOperator {{0x[0-9a-fA-F]+}} <line:7:7, col:11> 'bool' ==
// CHECK-NEXT: |   | |-ImplicitCastExpr {{0x[0-9a-fA-F]+}} <line:7:7> 'int' <LValueToRValue>
// CHECK-NEXT: |   | | `-DeclRefExpr {{0x[0-9a-fA-F]+}} <line:7:7> 'int' lvalue ParmVar {{0x[0-9a-fA-F]+}} 'n' 'int'
// CHECK-NEXT: |   | `-IntegerLiteral {{0x[0-9a-fA-F]+}} <line:7:11> 'int' 0
// CHECK-NEXT: |   |-ReturnStmt {{0x[0-9a-fA-F]+}} <line:7:17>
// CHECK-NEXT: |   | `-IntegerLiteral {{0x[0-9a-fA-F]+}} <line:7:24> 'int' 1
// CHECK-NEXT: |   `-CompoundStmt {{0x[0-9a-fA-F]+}} <line:7:16, line:8:1>
// CHECK-NEXT: |     `-ReturnStmt {{0x[0-9a-fA-F]+}} <line:8:5>
// CHECK-NEXT: |       `-BinaryOperator {{0x[0-9a-fA-F]+}} <line:8:12, col:14> 'int' *
// CHECK-NEXT: |         |-ImplicitCastExpr {{0x[0-9a-fA-F]+}} <line:8:12> 'int' <LValueToRValue>
// CHECK-NEXT: |         | `-DeclRefExpr {{0x[0-9a-fA-F]+}} <line:8:12> 'int' lvalue ParmVar {{0x[0-9a-fA-F]+}} 'n' 'int'
// CHECK-NEXT: |         `-CallExpr {{0x[0-9a-fA-F]+}} <line:8:12, col:14> 'int'
// CHECK-NEXT: |           |-ImplicitCastExpr {{0x[0-9a-fA-F]+}} <line:8:12> 'int (*)(int)' <FunctionToPointerDecay>
// CHECK-NEXT: |           | `-DeclRefExpr {{0x[0-9a-fA-F]+}} <line:8:12> 'int (int)' lvalue Function {{0x[0-9a-fA-F]+}} 'factorial' 'int (int)'
// CHECK-NEXT: |           `-BinaryOperator {{0x[0-9a-fA-F]+}} <line:8:12, col:14> 'int' -
// CHECK-NEXT: |             |-ImplicitCastExpr {{0x[0-9a-fA-F]+}} <line:8:12> 'int' <LValueToRValue>
// CHECK-NEXT: |             | `-DeclRefExpr {{0x[0-9a-fA-F]+}} <line:8:12> 'int' lvalue ParmVar {{0x[0-9a-fA-F]+}} 'n' 'int'
// CHECK-NEXT: |             `-IntegerLiteral {{0x[0-9a-fA-F]+}} <line:8:14> 'int' 1
int factorial(int n) {
  if (n == 0)
    return 1;
    return n * factorial(n - 1);
}

// CHECK: FunctionDecl {{0x[0-9a-fA-F]+}} <line:11:1, line:13:1> line:11:5 add 'int (int, int)'
// CHECK-NEXT: |-ParmVarDecl {{0x[0-9a-fA-F]+}} <col:13, col:17> col:17 used a 'int'
// CHECK-NEXT: |-ParmVarDecl {{0x[0-9a-fA-F]+}} <col:20, col:24> col:24 used b 'int'
// CHECK-NEXT: |-CompoundStmt {{0x[0-9a-fA-F]+}} <col:27, line:13:1>
// CHECK-NEXT: | `-ReturnStmt {{0x[0-9a-fA-F]+}} <line:12:3, col:11>
// CHECK-NEXT: |   `-BinaryOperator {{0x[0-9a-fA-F]+}} <col:10, col:11> 'int' '+'
// CHECK-NEXT: |     |-ImplicitCastExpr {{0x[0-9a-fA-F]+}} <col:10> 'int' <LValueToRValue>
// CHECK-NEXT: |     | `-DeclRefExpr {{0x[0-9a-fA-F]+}} <col:10> 'int' lvalue ParmVar {{0x[0-9a-fA-F]+}} 'a' 'int'
// CHECK-NEXT: |     `-ImplicitCastExpr {{0x[0-9a-fA-F]+}} <col:11> 'int' <LValueToRValue>
// CHECK-NEXT: |       `-DeclRefExpr {{0x[0-9a-fA-F]+}} <col:11> 'int' lvalue ParmVar {{0x[0-9a-fA-F]+}} 'b' 'int'
int add(int a, int b) {
  return a + b;
}

// Function with multiple return statements and attributes
// CHECK: FunctionDecl {{0x[0-9a-fA-F]+}} <line:16:1, line:21:1> line:16:6 checkRange 'int (int)'
// CHECK-NEXT: |-ParmVarDecl {{0x[0-9a-fA-F]+}} <col:17, col:21> col:21 used value 'int'
// CHECK-NEXT: |-CompoundStmt {{0x[0-9a-fA-F]+}} <col:24, line:21:1>
// CHECK-NEXT: | `-IfStmt {{0x[0-9a-fA-F]+}} <line:17:3, line:18:5>
// CHECK-NEXT: |   |-BinaryOperator {{0x[0-9a-fA-F]+}} <line:17:7, col:13> 'bool' <
// CHECK-NEXT: |   | |-ImplicitCastExpr {{0x[0-9a-fA-F]+}} <line:17:7> 'int' <LValueToRValue>
// CHECK-NEXT: |   | | `-DeclRefExpr {{0x[0-9a-fA-F]+}} <line:17:7> 'int' lvalue ParmVar {{0x[0-9a-fA-F]+}} 'value' 'int'
// CHECK-NEXT: |   | `-IntegerLiteral {{0x[0-9a-fA-F]+}} <line:17:13> 'int' 0
// CHECK-NEXT: |   |-ReturnStmt {{0x[0-9a-fA-F]+}} <line:18:5>
// CHECK-NEXT: |   | `-IntegerLiteral {{0x[0-9a-fA-F]+}} <line:18:12> 'int' -1
// CHECK-NEXT: | `-IfStmt {{0x[0-9a-fA-F]+}} <line:19:3, line:20:5>
// CHECK-NEXT: |   |-BinaryOperator {{0x[0-9a-fA-F]+}} <line:19:7, col:13> 'bool' >
// CHECK-NEXT: |   | |-ImplicitCastExpr {{0x[0-9a-fA-F]+}} <line:19:7> 'int' <LValueToRValue>
// CHECK-NEXT: |   | | `-DeclRefExpr {{0x[0-9a-fA-F]+}} <line:19:7> 'int' lvalue ParmVar {{0x[0-9a-fA-F]+}} 'value' 'int'
// CHECK-NEXT: |   | `-IntegerLiteral {{0x[0-9a-fA-F]+}} <line:19:13> 'int' 100
// CHECK-NEXT: |   |-ReturnStmt {{0x[0-9a-fA-F]+}} <line:20:5>
// CHECK-NEXT: |   | `-IntegerLiteral {{0x[0-9a-fA-F]+}} <line:20:12> 'int' 1
// CHECK-NEXT: | `-ReturnStmt {{0x[0-9a-fA-F]+}} <line:21:3>
// CHECK-NEXT: |   `-IntegerLiteral {{0x[0-9a-fA-F]+}} <line:21:10> 'int' 0
[[gnu::warn_unused_result]] int checkRange(int value) {
  if (value < 0)
    return -1;
  if (value > 100)
    return 1;
  return 0;
}

// CHECK: FunctionDecl {{0x[0-9a-fA-F]+}} <line:24:1, line:26:1> line:24:6 noexceptFunc 'void ()' noexcept
// CHECK-NEXT: |-CompoundStmt {{0x[0-9a-fA-F]+}} <col:23, line:26:1>
// CHECK-NEXT: | `-ReturnStmt {{0x[0-9a-fA-F]+}} <line:25:3>
void noexceptFunc() noexcept {
  return;
}
