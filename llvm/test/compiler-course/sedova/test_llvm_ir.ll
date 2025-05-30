; RUN: opt -load-pass-plugin %llvmshlibdir/ReplaceAddWithCall_Sedova_Olga_FIIT1_LLVM_IR%pluginext\
; RUN: -passes=replace-add -S %s | FileCheck %s

; CHECK-LABEL: define i32 @foo(i32 %x, i32 %y)
; CHECK: %sum = call i32 @add(i32 %x, i32 %y)
; CHECK: ret i32 %sum

; CHECK-LABEL: define i32 @add(i32 %a, i32 %b)
; CHECK: %result = add i32 %a, %b
; CHECK: ret i32 %result

; CHECK-LABEL: define i32 @bar(i32 %x, i32 %y)
; CHECK: %sum = add i32 %x, %y
; CHECK: ret i32 %sum

; CHECK-LABEL: define i64 @baz(i64 %x, i64 %y)
; CHECK: %sum = add i64 %x, %y
; CHECK: ret i64 %sum

define i32 @add(i32 %a, i32 %b) {
entry:
  %result = add i32 %a, %b
  ret i32 %result
}

define i32 @foo(i32 %x, i32 %y) {
entry:
  %sum = add i32 %x, %y
  ret i32 %sum
}

define i32 @bar(i32 %x, i32 %y) {
entry:
  %sum = add i32 %x, %y
  ret i32 %sum
}

define i64 @baz(i64 %x, i64 %y) {
entry:
  %sum = add i64 %x, %y
  ret i64 %sum
}