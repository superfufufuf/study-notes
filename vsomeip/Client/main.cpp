#include <thread>
#include <iostream>
#include "SomeIpTestClient.h"

using namespace std;

int main(int argc, char const *argv[])
{
    SomeIpTestClient client(0x1234, 0x5678, 0x1343);
    client.init();
    thread(std::bind(&SomeIpTestClient::start, &client)).detach();
    while (true)
    {
        string data;
        cout << "please input data." << endl;
        cin >> data;
        if (!client.SendMsg(data))
        {
            cout << "Send data failed, server is unavailable." << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(300));
    }
}