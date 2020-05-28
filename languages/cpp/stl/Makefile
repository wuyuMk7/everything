# Makefile for c++ stl library study

CXX=clang++
CXXFLAGS=-Wall

TARGETS=tuple unordered_map

all: $(TARGETS)

$(TARGETS): %: %.cc
	$(CXX) $(CXXFLAGS) -o $@.o -c $< 
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -rf *.o $(TARGETS) 



