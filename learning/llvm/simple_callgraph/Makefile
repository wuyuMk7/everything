CXX=clang++
LLVM_FLAGS=$(shell llvm-config --cxxflags --ldflags --system-libs --libs core)

all: callgraph

callgraph: main.cc CallGraph.o ControlFlow
	$(CXX) main.cc CallGraph.o $(LLVM_FLAGS) -o callgraph 

CallGraph.o: CallGraph.cc CallGraph.h
	$(CXX) -c CallGraph.cc -o CallGraph.o

ControlFlow: ControlFlow.cc ControlFlow.h
	$(CXX) ControlFlow.cc $(LLVM_FLAGS) -o ControlFlow

cleam:
	rm -rf callgraph *.o
