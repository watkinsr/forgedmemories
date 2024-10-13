.PHONY: build
build:
	mkdir -p build  && cd build && cmake -G "Ninja" -S ../ -B . && ninja

.PHONY: run
run:
	mkdir -p build  && cd build && cmake -G "Ninja" -S ../ -B . && ninja && cd ../ && ./build/forgedmemories
