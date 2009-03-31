#! /bin/sh
#set -x
#
# $Id: autotools.sh,v 1.3 2009/03/31 08:08:39 vtschopp Exp $
#

echo "Bootstrapping autotools..."

#echo "aclocal..."
#aclocal -I project
echo "libtoolize..."
libtoolize --force
echo "autoheader..."
autoheader
echo "automake..."
automake --foreign --add-missing --copy
echo "autoconf..."
autoconf

echo "Done."
