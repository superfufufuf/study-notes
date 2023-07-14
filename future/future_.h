#ifndef FUTURE_H
#define FUTURE_H

#include <memory>
#include <mutex>
#include <iostream>
#include <stdlib.h>
#include <list>

class FutureTest
{
public:
    FutureTest() = default;
    ~FutureTest();

    void test();
};

#endif