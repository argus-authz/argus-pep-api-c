#! /bin/sh
#set -x
#
# $Id: autotools.sh,v 1.2 2008/12/17 16:45:49 vtschopp Exp $
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
