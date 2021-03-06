#ifndef STIMER_H
#define STIMER_H

#include <stdint.h>

typedef uint32_t counter_t;

class STimer
{
public:

    volatile static counter_t static_timer_count;

    counter_t period; //in ms
    counter_t offset;
    bool working;

    STimer();

    void setPeriod(counter_t period);

    bool check();

    void stop();

    void start();

    counter_t getTime();

    inline static void clock(){
        static_timer_count++;
    }
};

#endif