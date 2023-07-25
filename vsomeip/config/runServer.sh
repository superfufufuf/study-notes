#! /bin/bash

#if your vsomeip application not set application name, it will use VSOMEIP_APPLICATION_NAME to set applcation name.
#If there are many hosts running the same program in the LAN, it is still necessary to set the program name by configuring VSOMEIP_CONFIGURATION
#export VSOMEIP_APPLICATION_NAME=TestA
export VSOMEIP_CONFIGURATION=`pwd`/vsomeip-local.json

./SomeIpTestServer