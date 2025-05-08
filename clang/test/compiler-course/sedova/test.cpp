// RUN: %clang_cc1 -load %llvmshlibdir/Lab1_Sedova_Olga_FIIT1_ClangAST%pluginext -plugin Lab1 -fsyntax-only %s 2>&1 | FileCheck %s

// CHECK: FunctionDecl {{.*}} isPositive 'bool (int)'
// CHECK-NEXT: |-ParmVarDecl {{.*}} a 'int'
// CHECK-NEXT: `-CompoundStmt

bool isPositive(int a) {
  return a > 0;
}

