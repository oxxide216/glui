#!/usr/bin/sh

CFLAGS="-Wall -Wextra -Isrc"
LDFLAGS="-z execstack -lX11 -lGL -lGLEW"
BUILD_FLAGS="${@:1}"
SRC="$(find src -name "*.c")"
GLASS_SRC="$(find libs/glass/src -name "*.c")"

cc -o glui-test $CFLAGS $LDFLAGS $BUILD_FLAGS $SRC $GLASS_SRC
