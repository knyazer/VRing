#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <iostream>

using namespace std;
using namespace chrono;

class Timer
{
public:
    steady_clock::time_point start = steady_clock::now();

    void begin()
    {
        start = steady_clock::now();
    }

    void clear()
    {
        begin();
    }

    long long ns()
    {
        return duration_cast<nanoseconds>(steady_clock::now() - start).count();
    }

    long long ms()
    {
        return duration_cast<milliseconds>(steady_clock::now() - start).count();
    }
};

#endif