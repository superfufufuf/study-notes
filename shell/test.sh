#!/bin/bash

SRCDIR=`dirname $0`
CURDIR=`pwd`

strip_arg()
{
	echo "$0 $1 $2"
	x=`echo "$1" | sed "s/[^=]*=//"`
	eval $2=\"$x\"
}

warn() {
	echo "$*" 1>&2
}

find_prog() {
	IFS=:
	for p in $PATH; do
		if [ -x ${p}/$1 ]
		then
			unset IFS
			echo "${p}/${1}"
			# printf "%s" "$IFS"
			return 0
		fi
	done
	unset IFS
	return 1
}

need_cmake() {
	warn "This program requires CMake 2.6 or newer to build."
	warn "Obtain CMake from www.cmake.org"
	exit 1
}

help() {
	echo "This configure script is a convenience wrapper around"
	echo "CMake.  Only a limited subset of configuration options"
	echo "are supported.  For a fully featured build, use CMake".
	echo
	echo "Execute as $0 [options]"
	echo
	echo "Options supported are:"
	echo
	echo " --prefix={path}      Set installation directory prefix."
	echo " --with-cmake={path}  Location of CMake executable."
	echo
	exit 1
}

# Argument parsing
PREFIX=/usr/local
while [ -n "$1" ]; do
	case "$1" in
	--with-cmake)
		CMAKE="$2"
		shift 2
		;;
	--with-cmake=*)
		strip_arg "$1" CMAKE
		shift
		;;
	--prefix)
		if [ -z "$2" ]; then
			help
		fi
		PREFIX="$2"
		shift 2
		;;
	--prefix=*)
		strip_arg "$1" PREFIX
		shift
		;;
	*)
		help
		;;
	esac
done

# find_prog cmake
if [ -z "${CMAKE}" ]; then
	CMAKE=`find_prog cmake` || need_cmake
fi

GENERATOR="Unix Makefiles"

echo ""$CMAKE" -G "$GENERATOR" "-DCMAKE_INSTALL_PREFIX=$PREFIX" $SRCDIR"
#"$CMAKE" -G "$GENERATOR" "-DCMAKE_INSTALL_PREFIX=$PREFIX" $SRCDIR
