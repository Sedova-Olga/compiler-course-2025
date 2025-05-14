// RUN: mlir-opt -load-pass-plugin=%mlir_lib_dir/Lab4_Sedova_Olga_FIIT1_MLIR%shlibext \
// RUN: --pass-pipeline="builtin.module(Lab4_Sedova_Olga_FIIT1_MLIR)" %s | FileCheck %s

// CHECK-LABEL: func @test_affine
// CHECK: func.call @trace_loop_iter_begin()
// CHECK: affine.for
// CHECK: func.call @trace_loop_iter_end()

// CHECK-LABEL: func @test_scf_for
// CHECK: func.call @trace_loop_iter_begin()
// CHECK: scf.for
// CHECK: func.call @trace_loop_iter_end()

// CHECK-LABEL: func @test_scf_while
// CHECK: func.call @trace_loop_iter_begin()
// CHECK: scf.while
// CHECK: func.call @trace_loop_iter_end()

func.func @trace_loop_iter_begin() {
  return
}

func.func @trace_loop_iter_end() {
  return
}

func.func @test_affine(%arg0: memref<10xf32>) {
  affine.for %i = 0 to 10 {
    // trace_loop_iter_begin should be inserted here by the pass
    %val = affine.load %arg0[%i] : memref<10xf32>
    // trace_loop_iter_end should be inserted here by the pass
  }
  return
}

func.func @test_scf_for(%arg0: memref<10xf32>) {
%c0 = arith.constant 0 : index
%c10 = arith.constant 10 : index
%c1 = arith.constant 1 : index
  scf.for %i = %c0 to %c10 step %c1  {
    // trace_loop_iter_begin should be inserted here by the pass
    %val = memref.load %arg0[%i] : memref<10xf32>
    // trace_loop_iter_end should be inserted here by the pass
  }
  return
}

func.func @test_nested_loops(%arg0: memref<10xf32>) {
  %c0 = arith.constant 0 : index
  %c5 = arith.constant 5 : index
  %c10 = arith.constant 10 : index
  %c1 = arith.constant 1 : index

  affine.for %i = 0 to 5 {
    // CHECK: func.call @trace_loop_iter_begin()
    affine.for %j = 0 to 10 {
      // CHECK: func.call @trace_loop_iter_begin()
      %val = memref.load %arg0[%j] : memref<10xf32>
      // CHECK: func.call @trace_loop_iter_end()
    }
    // CHECK: func.call @trace_loop_iter_end()
  }
  return
}

func.func @test_scf_if_in_loop(%arg0: memref<10xf32>) {
  %c0 = arith.constant 0 : index
  %c10 = arith.constant 10 : index
  %c1 = arith.constant 1 : index

  scf.for %i = %c0 to %c10 step %c1 {
    // CHECK: func.call @trace_loop_iter_begin()
    scf.if %true {
      %val = memref.load %arg0[%i] : memref<10xf32>
    }
    // CHECK: func.call @trace_loop_iter_end()
  }
  return
}

func.func @test_scf_while(%arg0: memref<10xf32>) {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c4 = arith.constant 4 : index
    %lhs = memref.alloc() : memref<4x4xi32>
    %rhs = memref.alloc() : memref<4x4xi32>
    %res = memref.alloc() : memref<4x4xi32>

    scf.while(%i = %c0) : (index) -> (index) {
      %cond = arith.cmpi ult, %i, %c4 : index
      scf.condition(%cond) %i : index
    } do {
    ^bb0(%i: index):
      scf.while(%j = %c0) : (index) -> (index) {
        %cond = arith.cmpi ult, %j, %c4 : index
        scf.condition(%cond) %j : index
      } do {
      ^bb1(%j: index):
        scf.while(%k = %c0) : (index) -> (index) {
          %cond = arith.cmpi ult, %k, %c4 : index
          scf.condition(%cond) %k : index
        } do {
        ^bb2(%k: index):
          %load_lhs = memref.load %lhs[%i, %k] : memref<4x4xi32> // lhs[i][k]
          %load_rhs = memref.load %rhs[%k, %j] : memref<4x4xi32> // rhs[k][j]
          %load_res = memref.load %res[%i, %j] : memref<4x4xi32> // res[i][j]
          %mul = arith.muli %load_lhs, %load_rhs : i32           // mul = lhs[i][k] * rhs[k][j]
          %sum = arith.addi %load_res, %mul : i32                // sum = res[i][j] + mul
          memref.store %sum, %res[%i, %j] : memref<4x4xi32>      // res[i][j] = sum
          %incr = arith.addi %k, %c1 : index
          scf.yield %incr : index
        }
        %incr = arith.addi %j, %c1 : index
        scf.yield %incr : index
      }
      %incr = arith.addi %i, %c1 : index
      scf.yield %incr : index
    }
    return
  }
