#include <cstdlib>
#include <ctime>
#include <random>
#include <chrono>
#include <iostream>

void testRand()
{
    srand(std::time(0));

    auto nowTime = std::chrono::steady_clock::now();
    for (int i = 0; i < 1000000; i++)
    {
        rand();
    }
    std::cout << "rand use time: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - nowTime).count() << "ms." << std::endl;
}

void testRandom()
{
    std::mt19937 engine{std::random_device{}()};
    std::uniform_int_distribution<int> distribution(0, 65535);
    auto nowTime = std::chrono::steady_clock::now();
    for (int i = 0; i < 1000000; i++)
    {
        distribution(engine);
    }
    std::cout << "random use time: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - nowTime).count() << "ms." << std::endl;
}