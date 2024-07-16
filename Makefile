CXX      := g++
CXXFLAGS := -pedantic-errors -Wno-sign-compare -Wno-unknown-pragmas -Wall -Wextra -std=c++17 -Og -g
LDFLAGS  := -L/usr/lib -L/usr/local/lib/ -lstdc++ -lm -lz3 -lcudd -lepd -lldd -lmtr -lst -ldddmp -llddutil
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)
TARGET   := fperf
INCLUDE  := -I/usr/local/include -Ilib/ -Ilib/metrics/ -Ilib/cps -Ilib/qms -I/Users/lucedes/Documents/GitHub/ldd/src/include -I/Users/lucedes/Documents/GitHub/ldd/cudd-2.4.2/include

SRC      :=	$(wildcard src/*.cpp) \
						 $(wildcard src/*/*.cpp)
TEST_SRC := $(wildcard tests/*.cpp)
TEST_TARGET_PATH := $(APP_DIR)/$(TARGET)_test
			
HEADERS := $(patsubst src/%.cpp,lib/%.hpp, $(filter-out src/main.cpp, $(SRC)))
OBJECTS := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS)

.PHONY: all build clean test check-format format

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

run:
	./$(APP_DIR)/$(TARGET)

$(TEST_TARGET_PATH): $(OBJECTS) $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $(TEST_SRC) $(filter-out ./build/objects/src/main.o, $(OBJECTS)) $(LDFLAGS)

test: $(TEST_TARGET_PATH)
	$^

check-format: $(HEADERS) $(SRC)
	clang-format --dry-run $^

format: $(HEADERS) $(SRC)
	clang-format -i $^

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*
