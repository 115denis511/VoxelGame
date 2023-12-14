#include "Log.h"

std::string engine::Log::g_log[engine_properties::MAX_LOG_STRINGS];
int         engine::Log::g_lastPos = -1;
bool        engine::Log::g_overflow = false;
bool        engine::Log::g_haveFatalError = false;
std::mutex  engine::Log::g_mutex;

void engine::Log::addMessage(std::string message) {
    g_mutex.lock();

    utilites::SystemTime currentTime;
    const std::string timeStr = "[" + 
        currentTime.getHourStr() + ":" + 
        currentTime.getMinuteStr() + ":" + 
        currentTime.getSecondStr() + "] ";

    int prevLastPos = g_lastPos;

    g_lastPos = (g_lastPos + 1) % engine_properties::MAX_LOG_STRINGS;
    g_log[g_lastPos] = timeStr + message;

    if (!g_overflow) {
        g_overflow = prevLastPos > g_lastPos;
    }

    g_mutex.unlock();
}

void engine::Log::addFatalError(std::string message) {
    g_haveFatalError = true;
    addMessage(message);
}

bool engine::Log::isHaveFatalError() {
    return g_haveFatalError;
}

void engine::Log::printInConsole() {
    g_mutex.lock();

    if (g_lastPos == -1) {
        addMessage("Log is empty");
    }

    if (g_overflow) {
        for (size_t i = 0; i < engine_properties::MAX_LOG_STRINGS; i++) {
            size_t index = (g_lastPos + 1 + i) % engine_properties::MAX_LOG_STRINGS;
            std::cout << g_log[index] << "\n";
        }
    }
    else {
        for (int i = 0; i <= g_lastPos; i++) {
            std::cout << g_log[i] << "\n";
        }
    }
    
    g_mutex.unlock();
}

void engine::Log::save() {
    g_mutex.lock();

    const std::string logFolder("Log");
    if (!std::filesystem::exists(logFolder)) {
        std::filesystem::create_directory(logFolder);
    }

    utilites::SystemTime currentTime;
    const std::string logFileName = 
        currentTime.getYearStr() + "-" + 
        currentTime.getMonthStr() + "-" + 
        currentTime.getDayStr() + "_" + 
        currentTime.getHourStr() + "-" + 
        currentTime.getMinuteStr() + "-" + 
        currentTime.getSecondStr();

    std::ofstream out(logFolder + "/" + logFileName + " log.txt");

    if (g_lastPos == -1) {
        addMessage("Log is empty");
    }

    if (g_overflow) {
        for (size_t i = 0; i < engine_properties::MAX_LOG_STRINGS; i++) {
            size_t index = (g_lastPos + 1 + i) % engine_properties::MAX_LOG_STRINGS;
            out << g_log[index] << "\n";
        }
    }
    else {
        for (int i = 0; i <= g_lastPos; i++) {
            out << g_log[i] << "\n";
        }
    }

    out.close();
    
    g_mutex.unlock();
}
