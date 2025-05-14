// RUN: mlir-opt --load-dialect=func --load-dialect=scf --load-dialect=arith --load-pass-plugin=%mlir_lib_dir/Lab4_Sedova_Olga_FIIT1_MLIR%shlibext \
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

func.func @test_scf_while(%arg0: memref<10xf32>) {
  %c0 = arith.constant 0 : index
  %c10 = arith.constant 10 : index
  %c1 = arith.constant 1 : index
  scf.while (%iv = %c0) : (index) -> () {
     scf.condition(%cond, %iv) : (i1, index) {
      %cond = arith.cmpi slt, %iv, %c10 : index
      scf.yield %cond : i1
    } do {
      // trace_loop_iter_begin should be inserted here by the pass
      %val = memref.load %arg0[%iv] : memref<10xf32>
      // trace_loop_iter_end should be inserted here by the pass
      %iv_next = arith.addi %iv, %c1 : index
      scf.yield %iv_next : index
    }
  }
  return
}