.PHONY: all install-deps build init

all: init install-deps build

init:
	conan profile detect --force

install-deps:
	conan install . --build=missing -s compiler.cppstd=20
	cp build/Release/generators/CMakePresets.json .

build:
	cmake --preset conan-release
	cmake --build --preset conan-release --parallel