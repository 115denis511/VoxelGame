#ifndef __UTILITES__SYSTEM_TIME_H__
#define __UTILITES__SYSTEM_TIME_H__

#include "../stdafx.h"
#include <ctime>

namespace utilites {
    class SystemTime {
    public:
        SystemTime();
        ~SystemTime();

        int getYear();
        int getMonth();
        int getDay();
        int getHour();
        int getMinute();
        int getSecond();
        std::string getYearStr();
        std::string getMonthStr();
        std::string getDayStr();
        std::string getHourStr();
        std::string getMinuteStr();
        std::string getSecondStr();

    private:
        tm m_ctime;
    };
}

#endif