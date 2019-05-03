# Makefile

CXXFLAGS := -Wall -std=c++11 -g
LIBS := -ldl

TARGET := content-search
LIB_DIR := libs
OBJECTS := $(addsuffix .o, $(basename $(wildcard */*.cc)))
LIBRARIES := $(LIB_DIR)/textSearch.so $(LIB_DIR)/fileManager.so
SHARED_LIBRARY_OBJECTS := 

define update_shared_library_objects
	$(if $(filter fileManager.so, $(1)), \
		$(eval SHARED_LIBRARY_OBJECTS=fileManager/FileSystem.o), \
		$(eval SHARED_LIBRARY_OBJECTS=) \
	)
endef

all: $(TARGET)

$(TARGET): $(TARGET).cc $(OBJECTS) $(LIBRARIES)
	$(CXX) $(CXXFLAGS) $< $(LIBRARIES) -o $(TARGET) $(LIBS)

$(OBJECTS): %.o : %.cc
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

# .SECONDEXPANSION:
# $(LIBRARIES) : $(LIB_DIR)/%.so : $$*/%.o
# 	@mkdir -p $(LIB_DIR)
# 	$(call update_shared_library_objects,$(notdir $@))
# 	$(CXX) $(CXXFLAGS) -fPIC -c $(addsuffix .cc, $(basename $<)) -o $<
# 	$(CXX) $(CXXFLAGS) -shared $< $(SHARED_LIBRARY_OBJECTS) -o $@

.SECONDEXPANSION:
$(LIBRARIES) : $(LIB_DIR)/%.so : $$*/%.o
	@mkdir -p $(LIB_DIR)
	$(call update_shared_library_objects,$(notdir $@))
	$(CXX) $(CXXFLAGS) -shared $< $(SHARED_LIBRARY_OBJECTS) -o $@

clean:
	rm -rf $(LIB_DIR) $(TARGET) *.so *.o **/*.o **/*.so
