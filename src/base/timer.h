#ifndef ZLPP_TIMER_H_
#define ZLPP_TIMER_H_

#ifndef TIMER_SMALL_SECOND
    #define TIMER_SMALL_SECOND 5e-324
#endif

#ifndef TIMER_LARGE_SECOND
    #define TIMER_LARGE_SECOND 5e324
#endif

#include <ctime>

namespace zh
{
class Timer
{
  public:
    /* brief A type for seconds */
    typedef double Second;
    /* brief A type for representing clock ticks */
    typedef long long unsigned Cycle;

  private:
    Cycle beginning;

    Cycle ending;

    Second beginningS;

    Second endingS;

    static Cycle rdtsc();

    bool running;

  public:
    Timer();

    void start();

    void stop();

    Cycle cycles() const;

    Second seconds() const;

    Second absolute() const;
};
}

#endif