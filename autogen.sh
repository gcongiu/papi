#!/bin/sh
echo_n() {
    printf "%s" "$*"
}

# external libraries autoreconf
external="external/libpfm4"

for i in $external; do
    echo "autoreconf `basename $external`... "
    cd $i && ./autogen.sh && cd - >& /dev/null
    echo "done"
    echo "==================================="
    echo ""
done

# we need to create a place holder so that autoreconf
# will not complain. Later configure fills the Makefile
# with components Makefile files.
touch src/components/Makefile.mk

echo "autoreconf papi..."
autoreconf -ivf
echo "done"
echo "==================================="
