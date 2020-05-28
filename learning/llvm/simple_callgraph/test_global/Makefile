all: global_test

global_test: module1.c module2.c inline.h
	clang -c module1.c -o module1.o
	clang -c module2.c -o module2.o
	clang -O2 -S -emit-llvm module1.c -o module1.ll
	clang -O2 -S -emit-llvm module2.c -o module2.ll
	llvm-as module1.ll -o module1.bc
	llvm-as module2.ll -o module2.bc
	clang module1.o module2.o -o global_test

clean:
	rm -rf *.o global_test *.ll *.bc
