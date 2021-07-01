#! /bin/sh
# Use this script if you add paths to LD_LIBRARY_PATH
# that contain libraries that conflict with the
# libraries that rpu designer depends on.
# author:zhangjun
# version:1.0.0
# release:2016/12/22

makeAbsolute() {
    case $1 in
        /*)
            # already absolute, return it
            echo "$1"
            ;;
        *)
            # relative, prepend $2 made absolute
            echo `makeAbsolute "$2" "$PWD"`/"$1" | sed 's,/\.$,,'
            ;;
    esac
}

me=`which "$0"` # Search $PATH if necessary
if test -L "$me"; then
    # Try readlink(1)
    readlink=`type readlink 2>/dev/null` || readlink=
    if test -n "$readlink"; then
        # We have readlink(1), so we can use it. Assuming GNU readlink (for -f).
        me=`readlink -nf "$me"`
    else
        # No readlink(1), so let's try ls -l
        me=`ls -l "$me" | sed 's/^.*-> //'`
        base=`dirname "$me"`
        me=`makeAbsolute "$me" "$base"`
    fi
fi

bindir=`dirname "$me"`
libdir=`cd "$bindir/../lib" ; pwd`
# Add rpu designer library path
LD_LIBRARY_PATH=$libdir:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH
exec "$bindir/firmwaretool" ${1+"$@"} >/dev/null 2>&1 &
