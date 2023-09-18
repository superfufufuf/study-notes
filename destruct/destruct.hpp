#ifndef DESTRUCT_TEST_H
#define DESTRUCT_TEST_H

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <string>

using namespace std;

void exitFun()
{
    cout << "main is exit" << endl;
}

void abnormalExit(int32_t id)
{
    cout << "get one sign:" << id << endl;
}

void TestDestruct()
{
    atexit(exitFun);
    uint8_t exit_group[] = {
        SIGHUP,
        SIGINT,
        SIGQUIT,
        SIGILL,
        SIGTRAP,
        SIGBUS,
        SIGSEGV,
        SIGFPE,
        SIGIOT,
        SIGKILL,
        /*SIGPIPE,*/ SIGTERM,
        SIGSTOP,
        SIGTSTP,
        SIGTTOU,
        SIGTTIN,
        SIGXFSZ,
        SIGPROF,
        SIGUSR1,
        SIGUSR2,
        SIGVTALRM,

    };

    for (auto i : exit_group)
    {
        ::signal(i, abnormalExit);
    }
}

class DestructChecker
{
public:
    DestructChecker(const string &_key)
        : key(_key)
    {
        cout << key << " is create." << endl;
    }

    ~DestructChecker()
    {
        cout << key << " is destory." << endl;
    }

    void checkAlive()
    {
    }

private:
    string key;
};

#endif