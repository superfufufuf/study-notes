#! /bin/bash
cur_dir=$(dirname $(readlink -f "$0"))

cd ${cur_dir}

fidlList=$(find $cur_dir -name "*.fidl")
for fidlFile in $fidlList
do
    echo "generate core file ${fidlFile}"
    commonapi-core-generator-linux-x86_64 -sk ${fidlFile}
done;

someIpFdeplList=$(find $cur_dir -name "*SomeIP.fdepl")
for someIpFile in $someIpFdeplList
do
    echo "generate someIp file ${someIpFile}"
    commonapi-someip-generator-linux-x86_64 ${someIpFile}
done;

# DBusFdeplList=$(find $cur_dir -name "*DBus.fdepl")
# for DBusFile in $DBusFdeplList
# do
#     echo "generate DbusIp file ${DBusFile}"
#     commonapi-dbus-generator-linux-x86_64 ${DBusFile}
# done;

rm -rf ../src-gen
mv src-gen ../