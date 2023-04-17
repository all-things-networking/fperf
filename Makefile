CXX      := g++
CXXFLAGS := -pedantic-errors -Wno-unknown-pragmas -Wall -Wextra -Werror -std=c++17 -O3 
LDFLAGS  := -L/usr/lib -L/usr/local/lib/ -lstdc++ -lm -lz3
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/
TARGET   := autoperf
INCLUDE  := -I/usr/local/include -Ilib/ -Ilib/metrics/ -Ilib/cps -Ilib/qms
SRC      :=	$(wildcard src/*.cpp) \
					  $(wildcard src/metrics/*.cpp) \
						$(wildcard src/cps/*.cpp) \
						$(wildcard src/qms/*.cpp)
			

OS_NAME := $(shell uname -s | tr A-Z a-z)

OBJECTS := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS)

.PHONY: all build clean

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

run:
	./build/autoperf
	
clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*
