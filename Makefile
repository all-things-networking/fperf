CXX      := g++
CXXFLAGS := -g -std=c++17 -O0 -w
LDFLAGS  := -L/usr/lib -L/usr/local/lib/ -lstdc++ -lm -lz3
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)
TARGET   := fperf
INCLUDE  := -I/usr/local/include -Ilib/ -Ilib/metrics/ -Ilib/cps -Ilib/qms
SRC      :=	$(wildcard src/*.cpp) \
					  $(wildcard src/metrics/*.cpp) \
						$(wildcard src/cps/*.cpp) \
						$(wildcard src/qms/*.cpp)
TEST_SRC := $(wildcard tests/*.cpp)
TEST_TARGET_PATH := $(APP_DIR)/$(TARGET)_test
			


OBJECTS := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS)

.PHONY: all build clean test

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

run:
	./$(APP_DIR)/$(TARGET)

$(TEST_TARGET_PATH): $(OBJECTS) $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $(TEST_SRC) $(filter-out ./build/objects/src/main.o, $(OBJECTS)) $(LDFLAGS)

test: $(TEST_TARGET_PATH)
	$^

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*
