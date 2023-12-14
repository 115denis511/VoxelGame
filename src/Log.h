#ifndef __LOG_H__
#define __LOG_H__

#include "stdafx.h"
#include <ctime>
#include "Utilites/SystemTime.h"
#include "engine_properties.h"

namespace engine {
    class Log {
    public:
        static void addMessage(std::string message);
        static void addFatalError(std::string message);
        static bool isHaveFatalError();
        static void printInConsole();
        static void save();

    private:
        Log() {}
        
        static std::string g_log[engine_properties::MAX_LOG_STRINGS];
        static int         g_lastPos;
        static bool        g_overflow;
        static bool        g_haveFatalError;
        static std::mutex  g_mutex;

    };
}

#endif