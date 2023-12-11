#include "SystemTime.h"

utilites::SystemTime::SystemTime() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);
    m_ctime = *localtime(&tt);
}

utilites::SystemTime::~SystemTime() {

}

int utilites::SystemTime::getYear() {
    return m_ctime.tm_year + 1900;
}

int utilites::SystemTime::getMonth() {
    return m_ctime.tm_mon + 1;
}

int utilites::SystemTime::getDay() {
    return m_ctime.tm_mday;
}

int utilites::SystemTime::getHour() {
    return m_ctime.tm_hour;
}

int utilites::SystemTime::getMinute() {
    return m_ctime.tm_min;
}

int utilites::SystemTime::getSecond() {
    return m_ctime.tm_sec;
}

std::string utilites::SystemTime::getYearStr() {
    return std::to_string(getYear());
}

std::string utilites::SystemTime::getMonthStr() {
    std::string result = std::to_string(getMonth());
    if (getMonth() < 10) 
        result = "0" + result;
    return result;
}

std::string utilites::SystemTime::getDayStr() {
    std::string result = std::to_string(getDay());
    if (getDay() < 10) 
        result = "0" + result;
    return result;
}

std::string utilites::SystemTime::getHourStr() {
    std::string result = std::to_string(getHour());
    if (getHour() < 10) 
        result = "0" + result;
    return result;
}

std::string utilites::SystemTime::getMinuteStr() {
    std::string result = std::to_string(getMinute());
    if (getMinute() < 10) 
        result = "0" + result;
    return result;
}

std::string utilites::SystemTime::getSecondStr() {
    std::string result = std::to_string(getSecond());
    if (getSecond() < 10) 
        result = "0" + result;
    return result;
}
