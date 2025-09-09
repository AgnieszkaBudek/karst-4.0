//
// Created by Agnieszka on 10/08/2025.
//

#ifndef KARST_4_0_LOGGER_H
#define KARST_4_0_LOGGER_H



#include <iostream>
#include <string>
#include <sstream>
#include <memory>

#include "src/all_enums.h"


namespace karst{

    enum class LogLevel {DEBUG_FULL, DEBUG_PS, DEBUG, INFO, WARNING, ERROR};
    template<>
    const std::map<LogLevel, std::string> EnumToString<LogLevel>::mapping = {
            {LogLevel::DEBUG_FULL,  "DEBUG_FULL"},
            {LogLevel::DEBUG_PS,    "DEBUG_PS"  },
            {LogLevel::DEBUG,       "DEBUG"     },
            {LogLevel::INFO,        "INFO"      },
            {LogLevel::WARNING,     "WARNING"   },
            {LogLevel::ERROR,       "ERROR"     }
    };

    constexpr LogLevel      logger_level_min  = LogLevel::DEBUG_PS;
    constexpr std::ostream& logger_output     = std::cout;

    template<LogLevel MinLevel>
    class Logger {
    public:

        explicit Logger(std::ostream& os)
                : out(os) {}

        template<LogLevel Level = LogLevel::INFO>
        void log(const std::string& message) {
            if constexpr (Level >= MinLevel)
                out << "[" << Level << "] " << message << "\n";
        }

        template<LogLevel Level = LogLevel::INFO>
        void pure_log(const std::string& message) {
            if constexpr (Level >= MinLevel)
                out << message;
        }

        template<LogLevel Level = LogLevel::INFO, typename T>
        void log_with_context(const T& obj, const std::string& message) {
            if constexpr (Level >= MinLevel)
                out << "[" << Level << "] "
                    << obj.get_context_info() << "\t" << message << "\n";
        }

        template<LogLevel Level = LogLevel::INFO, typename T>
        void log_state(const T& obj, const std::string& message) {
            if constexpr (Level >= MinLevel) {
                out << "[" << Level << "] "
                    << obj.get_state_info();
                out << " " << message << "\n";
            }
        }

    private:
        std::ostream& out;

    };
}


#endif //KARST_4_0_LOGGER_H
