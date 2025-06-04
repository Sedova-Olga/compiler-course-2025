; RUN: opt -load-pass-plugin %llvmshlibdir/ReplaceAddWithCall_Sedova_Olga_FIIT1_LLVM_IR%pluginext\
; RUN: -passes=replace-add -S %s | FileCheck %s

; CHECK-LABEL: define i32 @add_i32(
; CHECK-NEXT: entry:
; CHECK-NEXT:         add i32 %
; CHECK-NEXT:         ret i32

; CHECK-LABEL: define i32 @foo_i32(
; CHECK-NEXT: entry:
; CHECK-NEXT:         call i32 @add_i32(i32 %x, i32 %y)
; CHECK-NEXT:         ret i32

; CHECK-LABEL: define i64 @add_i64(
; CHECK-NEXT: entry:
; CHECK-NEXT:         add i64 %
; CHECK-NEXT:         ret i64

; CHECK-LABEL: define i64 @foo_i64(
; CHECK-NEXT: entry:
; CHECK-NEXT:         call i64 @add_i64(i64 %x, i64 %y)
; CHECK-NEXT:         ret i64

; CHECK-LABEL: define float @add_float(
; CHECK-NEXT: entry:
; CHECK-NEXT:         fadd float %
; CHECK-NEXT:         ret float

; CHECK-LABEL: define float @foo_float(
; CHECK-NEXT: entry:
; CHECK-NEXT:         call float @add_float(float %x, float %y)
; CHECK-NEXT:         ret float

; CHECK-LABEL: define i32 @bar_i32(
; CHECK-NEXT: entry:
; CHECK-NEXT:         add i32 %
; CHECK-NEXT:         ret i32

; CHECK-LABEL: define float @bar_float(
; CHECK-NEXT: entry:
; CHECK-NEXT:         fadd float %
; CHECK-NEXT:         ret float

define i32 @add_i32(i32 %a, i32 %b) {
entry:
  %result = add i32 %a, %b
  ret i32 %result
}

define i32 @foo_i32(i32 %x, i32 %y) {
entry:
  %sum = add i32 %x, %y
  ret i32 %sum
}

define i64 @add_i64(i64 %a, i64 %b) {
entry:
  %result = add i64 %a, %b
  ret i64 %result
}

define i64 @foo_i64(i64 %x, i64 %y) {
entry:
  %sum = add i64 %x, %y
  ret i64 %sum
}

define float @add_float(float %a, float %b) {
entry:
  %result = fadd float %a, %b
  ret float %result
}

define float @foo_float(float %x, float %y) {
entry:
  %sum = fadd float %x, %y
  ret float %sum
}

define i32 @bar_i32(i32 %x, i32 %y) {
entry:
  %sum = add i32 %x, %y
  ret i32 %sum
}

define float @bar_float(float %x, float %y) {
entry:
  %sum = fadd float %x, %y
  ret float %sum
}

