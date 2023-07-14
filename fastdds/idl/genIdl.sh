#! /bin/bash
cur_dir=$(dirname $(readlink -f "$0"))

cd ${cur_dir}

idlList=$(find $cur_dir -name "*.idl")
for idlFile in $idlList
do
    echo "generate ${idlFile}"
    fastddsgen ${idlFile} -d ${cur_dir}/../src-gen/ -replace
done;