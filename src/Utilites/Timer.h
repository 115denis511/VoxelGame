#ifndef __UTILITES__TIMER_H__
#define __UTILITES__TIMER_H__

#include "../stdafx.h"

namespace utilites {
    class Timer {
    public:
        Timer();
        ~Timer();

        void reset();
        void printElapsedMilliseconds();
        void printElapsedMicroseconds();
        void printElapsedNanoseconds();
        long long elapsedMilliseconds();
        long long elapsedMicroseconds();
        long long elapsedNanoseconds();

    private:
        std::chrono::time_point<std::chrono::system_clock> m_startTime;
    };
}

#endif