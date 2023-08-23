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
		PREFIX="$2"
		shift 2
		;;
	--prefix=*)
		strip_arg "$1" PREFIX
		shift
		;;
	*)
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
#!/bin/bash

# 定义颜色变量
RED='\E[1;31m'        # 红
GREEN='\E[1;32m'      # 绿
YELOW='\E[1;33m'      # 黄
BLUE='\E[1;34m'       # 蓝
PINK='\E[1;35m'       # 粉红
SHAN='\E[33;5m'       #黄色闪烁警示
RES='\E[0m'           # 清除颜色

#用 echo -e 来调用
echo -e "${SHAN} this is yelow flashing warning ${RES}"  # 黄色闪烁警示调用效果
echo -e "${GREEN} this is green color ${RES}"
echo -e "${YELOW} this is red yelow ${RES}"
echo -e "${BLUE} this is blue color ${RES}"
echo -e "${PINK} this is pink color ${RES}"
for i in {1..100}
do
    printf "\rProcess: %3d" $i
    sleep 1
done