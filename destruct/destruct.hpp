#ifndef DESTRUCT_TEST_H
#define DESTRUCT_TEST_H

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include "LogManager.h"

using namespace std;

void exitFun()
{
    _LOG("main is exit", LogLevel::DEBUG);
}

void abnormalExit(int32_t id)
{
    _LOG("get one sign:" + to_string(id), LogLevel::DEBUG);
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

#endif