#include <thread>
#include <iostream>
#include "SomeIpTestServer.h"

using namespace std;

int main(int argc, char const *argv[])
{
    SomeIpTestServer server(0x1234, 0x5678, 0x1343);
    server.init();
    server.start();
}