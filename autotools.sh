#! /bin/sh
#set -x
#
# $Id: autotools.sh,v 1.1 2008/12/12 11:27:12 vtschopp Exp $
#

echo "Bootstrapping autotool..."

#aclocal -I project
aclocal 
libtoolize --force
autoheader
automake --foreign --add-missing --copy
autoconf

