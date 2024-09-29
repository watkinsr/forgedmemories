#!/usr/bin/env sh

set -x
set -e

ARG0=$0
print_help() {
    NAME=$(basename "$ARG0")
    cat <<EOF
Usage: $NAME COMMAND
    $NAME build
        Builds the thing
EOF
}

die() {
    >&2 echo "die: $*"
    exit 1
}

usage() {
    >&2 print_help
    exit 1
}

build() {
    mkdir -p build;
    cd build;
    cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -S ../ -B .;
    ninja;    
}

CMD=$1
case "$CMD" in
    build)
	build
        ;;
    *)
        >&2 echo "Unknown command: $CMD"
        usage
        ;;
esac

