; ModuleID = 'module2.c'
source_filename = "module2.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@global_fptr1 = external local_unnamed_addr global i8*, align 8
@global_fptr2 = external local_unnamed_addr global i8*, align 8
@global_fptr3 = external local_unnamed_addr global void (...)*, align 8
@str = private unnamed_addr constant [13 x i8] c"In main now.\00", align 1
@str.6 = private unnamed_addr constant [13 x i8] c"ptr2-gfptr2:\00", align 1
@str.7 = private unnamed_addr constant [13 x i8] c"ptr3-gfptr2:\00", align 1
@str.8 = private unnamed_addr constant [8 x i8] c"gfptr3:\00", align 1
@str.9 = private unnamed_addr constant [13 x i8] c"ptr4-gfptr3:\00", align 1
@str.10 = private unnamed_addr constant [13 x i8] c"from inline!\00", align 1

; Function Attrs: nounwind sspstrong uwtable
define dso_local i32 @main(i32, i8** nocapture readnone) local_unnamed_addr #0 {
  %3 = tail call i32 @puts(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @str, i64 0, i64 0))
  %4 = tail call i32 @puts(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @str.10, i64 0, i64 0)) #1
  %5 = load void (i32)*, void (i32)** bitcast (i8** @global_fptr1 to void (i32)**), align 8, !tbaa !4
  tail call void %5(i32 20) #1
  %6 = tail call i32 @puts(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @str.6, i64 0, i64 0))
  %7 = load void (...)*, void (...)** bitcast (i8** @global_fptr2 to void (...)**), align 8, !tbaa !4
  tail call void (...) %7() #1
  %8 = tail call i32 @puts(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @str.7, i64 0, i64 0))
  %9 = load void (...)*, void (...)** bitcast (i8** @global_fptr2 to void (...)**), align 8, !tbaa !4
  tail call void (...) %9() #1
  %10 = tail call i32 @puts(i8* getelementptr inbounds ([8 x i8], [8 x i8]* @str.8, i64 0, i64 0))
  %11 = load void (...)*, void (...)** @global_fptr3, align 8, !tbaa !4
  tail call void (...) %11() #1
  %12 = tail call i32 @puts(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @str.9, i64 0, i64 0))
  %13 = load void (...)*, void (...)** @global_fptr3, align 8, !tbaa !4
  tail call void (...) %13() #1
  ret i32 0
}

; Function Attrs: nounwind
declare i32 @puts(i8* nocapture readonly) local_unnamed_addr #1

attributes #0 = { nounwind sspstrong uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{!"clang version 8.0.1 (tags/RELEASE_801/final)"}
!4 = !{!5, !5, i64 0}
!5 = !{!"any pointer", !6, i64 0}
!6 = !{!"omnipotent char", !7, i64 0}
!7 = !{!"Simple C/C++ TBAA"}
