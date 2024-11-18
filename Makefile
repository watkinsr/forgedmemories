.PHONY: build
build:
	mkdir -p build && \
    cd build && \
	cmake -DCMAKE_BUILD_TYPE=Debug -G "Ninja" -S ../ -B . && \
    ninja
