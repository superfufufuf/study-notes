#ifndef MY_CONST_H
#define MY_CONST_H

#include <string>
#include <list>
#include <map>
#include <iostream>
#include <thread>
#include <chrono>
#include <functional>

using namespace std;

struct Rect
{
    Rect() = default;
    Rect(const int _x, const int _y, const int _width, const int _height)
        : x(_x), y(_y), width(_width), height(_height)
    {
    }
    int x;
    int y;
    int width;
    int height;
};

#endif