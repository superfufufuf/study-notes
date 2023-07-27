#include <thread>
#include <iostream>
#include "SomeIpTestServer.h"

using namespace std;

int main(int argc, char const *argv[])
{
    SomeIpTestServer server;
    server.init();
    thread(std::bind(&SomeIpTestServer::start, &server)).detach();
    while (true)
    {
        string data;
        cout << "please input data." << endl;
        cin >> data;
        server.PublishData(data);
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}