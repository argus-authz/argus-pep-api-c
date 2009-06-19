#! /bin/sh
#set -x
#
# $Id$
#

echo "Bootstrapping autotools..."

echo "aclocal..."
aclocal -I project
echo "libtoolize..."
libtoolize --force
echo "autoheader..."
autoheader
echo "automake..."
automake --foreign --add-missing --copy
echo "autoconf..."
autoconf

echo "Done."
