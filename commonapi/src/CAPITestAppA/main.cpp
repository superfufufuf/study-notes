#include <thread>
#include <iostream>
#include "ModelA.h"
#include "ModelB.h"

using namespace std;

int main(int argc, char const *argv[])
{
    thread(ServerM).detach();
    thread(ClientN1).detach();
    thread(ClientN2).detach();

    cout << "all thread create complete" << endl;
    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}