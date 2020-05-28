OPTIMIZATION_FLAGS=
SRC_FILES=$(wildcard *.c)
TARGET=$(addsuffix .bc, $(basename $(SRC_FILES)))

all: $(TARGET)

optimization: OPTIMIZATION_FLAGS += -O3
optimization: $(TARGET)

# $(TARGET): %.bc: %.ll
# 	llvm-as $< -o $@
# 
# $(addsuffix .ll, $(basename $(SRC_FILES))): $(SRC_FILES)
# 	clang -S -emit-llvm $< -o $@

$(TARGET): %.bc: %.c
	clang $(OPTIMIZATION_FLAGS) -emit-llvm $< -c -o $@

clean:
	rm -rf *.ll *.bc


