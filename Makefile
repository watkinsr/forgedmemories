.PHONY: build
build:
	mkdir -p build  && cd build && cmake -G "Ninja" -S ../ -B . && ninja
