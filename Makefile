BUILD_TYPE?=Release
BUILD_DIR?=build

.PHONY: all
all: test

configure: CMakeLists.txt
	cmake -GNinja -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

build: configure src/*.cpp src/*.h
	cmake --build $(BUILD_DIR)

test: build
	ctest --test-dir $(BUILD_DIR)

package: test
	cd $(BUILD_DIR) && cpack -G ZIP

.PHONY: clean
clean:
	rm -Rf $(BUILD_DIR)