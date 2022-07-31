#!/usr/bin/env bash

set -e

ARG0=$0
print_help() {
    NAME=$(basename "$ARG0")
    cat <<EOF
Usage: $NAME COMMAND
    $NAME run
        Runs the thing
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

CMD=$1
case "$CMD" in
    build)
        mkdir -p build && cd build && cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -S ../ -B . && ninja;
        ;;
    run)
        ./target/rpg;
        ;;
    *)
        >&2 echo "Unknown command: $CMD"
        usage
        ;;
esac

