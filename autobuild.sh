#! /bin/bash
cur_dir=$(dirname $(readlink -f "$0"))

cpu_count=`cat /proc/cpuinfo| grep "processor"| wc -l`
use_count=$(($cpu_count - 1))

winSys="x11"

if [ $# -gt 0 ]; then
    winSys=$1
fi

build_dir=${cur_dir}/build
if [ "${winSys}" == "x11" ]; then
    echo "use x11 window system."
elif [ "${winSys}" == "wayland" ]; then
    build_dir=${cur_dir}/build_${winSys}
    echo "use wayland window system."
    #TODO you need source your cross-compiling enviornment
else
    echo "use other window system."
    build_dir=${cur_dir}/build_${winSys}
fi

echo "${build_dir}"
mkdir ${build_dir}
cd ${build_dir}

rm -rf CMake* cmake* Makefile .cmake
cmake ${build_dir}/../ -DWINSYS=${winSys}

if [ $cpu_count -gt 1 ]; then
    echo "make -j$use_count"
    make -j$use_count
else
    echo "make"
    make
fi
