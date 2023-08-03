#ifndef LIBRARY_TEST_H
#define LIBRARY_TEST_H

extern "C"
{
    int addTest(const int a, const int b);

    int subTest(const int a, const int b);
}

int divTest(const int a, const int b);

float divTest(const float a, const float b);

#endif