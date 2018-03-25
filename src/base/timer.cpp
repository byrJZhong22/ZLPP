#include "timer.h"

namespace zh
{
Timer::Timer()
{
    start();
}

void Timer::start()
{
    beginning = rdtsc();
    beginningS = (beginning + 0.0) * 1.0e-9;
    running = true;
}

void Timer::stop()
{
    ending = rdtsc();
    endingS = (ending + 0.0) * 1.0e-9;
    running = false;
}

Timer::Cycle Timer::cycles() const
{
    if (running)
    {
        return (rdtsc() - beginning);
    }
    else
    {
        return (ending - beginning);
    }
}

Timer::Second Timer::seconds() const
{
    if (running)
    {
        return (((rdtsc() + 0.0) * 1.0e-9) - beginningS);
    }
    else
    {
        return endingS - beginningS;
    }
}

Timer::Second Timer::absolute() const
{
    if (running)
    {
        return (((rdtsc() + 0.0) * 1.0e-9));
    }
    else
    {
        return endingS;
    }
}

Timer::Cycle Timer::rdtsc()
{
    timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);

    return spec.tv_nsec + (Cycle)spec.tv_sec * 1e9;
}
}