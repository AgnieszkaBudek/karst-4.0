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

    enum class LogLevel {DEBUG, INFO, WARNING, ERROR};
    template<>
    const std::map<LogLevel, std::string> EnumToString<LogLevel>::mapping = {
            {LogLevel::DEBUG,   "DEBUG"     },
            {LogLevel::INFO,    "INFO"      },
            {LogLevel::WARNING, "WARNING"   },
            {LogLevel::ERROR,   "ERROR"     }
    };

    namespace Logging {
        inline LogLevel minLevel = LogLevel::INFO;
    }

    class Logger {
    public:
        explicit Logger(std::ostream& os)
                : out(os) {}

        void log(LogLevel level, const std::string& message) {
            if (level < Logging::minLevel) return;
            out << "[" << level << "] " << message << "\n";
        }

        void pure_log(LogLevel level, const std::string& message) {
            if (level < Logging::minLevel) return;
            out << message;
        }

        template<typename T>
        void log_with_context(LogLevel level, const T& obj, const std::string& message) {
            if (level < Logging::minLevel) return;
            out << "[" << level << "] "
                << obj.get_context_info() << ": " << message << "\n";
        }

        template<typename T>
        void log_state(LogLevel level, const T& obj, const std::string& message) {
            if (level < Logging::minLevel) return;
            out << "[" << level << "] "
                << obj.get_context_info()
                << obj.get_state_info() << ": " << message << "\n";
        }



    private:
        std::ostream& out;



    };
//
//// ----- PRZYKŁADY PODREALIZACJI -----
//    inline Logger makeCoutLogger(LogLevel minLevel = LogLevel::Info) {
//        return Logger(std::cout, minLevel);
//    }
//
//    inline Logger makeCerrLogger(LogLevel minLevel = LogLevel::Info) {
//        return Logger(std::cerr, minLevel);
//    }
//
//// ----- PRZYKŁADOWE OBIEKTY TWOJEJ SIECI -----
//    struct Node {
//        int id;
//        std::string getContextInfo() const {
//            return "Node#" + std::to_string(id);
//        }
//    };
//
//    struct Pore {
//        int id;
//        std::string getContextInfo() const {
//            return "Pore#" + std::to_string(id);
//        }
//    };



}


#endif //KARST_4_0_LOGGER_H
