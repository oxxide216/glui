#!/usr/bin/sh

CFLAGS="-Wall -Wextra -Isrc -Ilibs -Ilibs/winx/src"
LDFLAGS="-z execstack -lX11 -lGL -lGLEW"
BUILD_FLAGS="${@:1}"
SRC="$(find src -name "*.c")"
WINX_SRC="$(find libs/winx/src -name "*.c")"
GLASS_SRC="$(find libs/glass/src -name "*.c")"

cc -o glui-test $CFLAGS $LDFLAGS $BUILD_FLAGS $SRC $WINX_SRC $GLASS_SRC
