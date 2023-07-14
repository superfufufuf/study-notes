#include <thread>
#include <iostream>
#include <fastrtps/Domain.h>
#include <fastrtps/log/Log.h>
#include "receiver.h"

using namespace std;
using namespace eprosima;
using namespace fastrtps;
using namespace rtps;

int main(int argc, char const *argv[])
{
    Receiver sender;
    sender.init();
    sender.run(10, 10);
    cout << "all thread create complete" << endl;
    // while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
    Domain::stopAll();
    Log::Reset();
}