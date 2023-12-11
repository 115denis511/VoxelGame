#include "Timer.h"

utilites::Timer::Timer() {
    reset();
}

utilites::Timer::~Timer() {

}

void utilites::Timer::reset() {
    m_startTime = std::chrono::system_clock::now();
}

void utilites::Timer::printElapsedMilliseconds() {
    std::cout << "Time in ms: " << (long double)elapsedMilliseconds() * 0.000001 << "\n";
}

void utilites::Timer::printElapsedMicroseconds() {
    std::cout << "Time in us: " << (long double)elapsedMicroseconds() * 0.000001 << "\n";
}

void utilites::Timer::printElapsedNanoseconds() {
    std::cout << "Time in ns: " << (long double)elapsedNanoseconds() * 0.000001 << "\n";
}

long long utilites::Timer::elapsedMilliseconds() {
    auto stopTime = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - m_startTime).count();
}

long long utilites::Timer::elapsedMicroseconds() {
    auto stopTime = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(stopTime - m_startTime).count();
}

long long utilites::Timer::elapsedNanoseconds() {
    auto stopTime = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(stopTime - m_startTime).count();
}