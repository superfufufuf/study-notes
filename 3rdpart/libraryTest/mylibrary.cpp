#include "mylibrary.h"

int addTest(const int a, const int b)
{
    return a + b;
}

int subTest(const int a, const int b)
{
    return a - b;
}

int divTest(const int a, const int b)
{
    return a / b;
}

float divTest(const float a, const float b)
{
    return a / b;
}

extern "C"
{
    int mulTest(const int a, const int b)
    {
        return a * b;
    }
}