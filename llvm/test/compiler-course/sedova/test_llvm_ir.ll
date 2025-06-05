; RUN: opt -load-pass-plugin %llvmshlibdir/ReplaceAddWithCall_Sedova_Olga_FIIT1_LLVM_IR%pluginext\
; RUN: -passes=replace-add -S %s | FileCheck %s

; CHECK: define i64 @mul(i64 %a, i64 %b)
; CHECK-NEXT: entry:
; CHECK-NEXT: %result = mul i64 %a, %b
; CHECK-NEXT: ret i64 %result

define i64 @mul(i64 %a, i64 %b) {
entry:
  %result =  mul i64 %a, %b
  ret i64 %result
}