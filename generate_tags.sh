#!/usr/bin/env sh

set -xe

find . \
     -type f \(-path "build" -o -path "bozohttpd" -o -path "assets" -prune\) \
     -name "*.cpp" -o -name "*.h" -print \
    | xargs /usr/bin/ctags -e -o TAGS {} +
