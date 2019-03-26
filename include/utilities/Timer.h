#ifndef SIGMODBENCHMARKS_TIMER_H
#define SIGMODBENCHMARKS_TIMER_H

#include <iostream>
#include <chrono>
#include <ctime>
#include <cmath>

class Timer {

    typedef std::chrono::high_resolution_clock Clock;
private:
    Clock::time_point start_time;
    Clock::time_point end_time;
    bool running = false;
public:
    void run() {
        start_time = Clock::now();
        running = true;
    }

    void stop() {
        end_time = Clock::now();
        running = false;
    }


    double elapsedNanoseconds() {
        Clock::time_point end_time;

        if(running) {
            end_time = Clock::now();
        }
        else {
            end_time = this->end_time;
        }

        return std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    }

    double elapsedMilliseconds() {
        return elapsedNanoseconds() / 1000000.0;
    }
};


#endif
