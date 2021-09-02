#!/bin/sh
echo_n() {
    printf "%s" "$*"
}

# we need to create a place holder so that autoreconf
# will not complain. Later configure fills the Makefile
# with components Makefile files.
touch src/components/Makefile.mk

autoreconf -ivf
