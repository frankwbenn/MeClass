#pragma once
#include <iostream>
#include <format>
#include "cgd_utils.h"

//NOTE: When printing
//log is used for logging warnings, errors, and 
//controlling all terminal print statements.
//TODO: logging to files.
//TODO: Replace function pointers with switch statement if no other use for them comes up.

#define LOG_ERROR(fmt, ...) \
    Log::Get().LogMessageDebug(Log::Error, __builtin_FILE(), __builtin_FUNCTION(), __builtin_LINE(), \
            fmt, ##__VA_ARGS__)

#define LOG_WARN(fmt, ...) \
    Log::Get().LogMessageDebug(Log::Warn, __builtin_FILE(), __builtin_FUNCTION(), __builtin_LINE(), \
            fmt, ##__VA_ARGS__)

#define LOG_INFO(fmt, ...) \
    Log::Get().LogMessageDebug(Log::Info, __builtin_FILE(), __builtin_FUNCTION(), __builtin_LINE(), \
            fmt, ##__VA_ARGS__)

#define PRINT(fmt, ...) Log::Get().LogMessageForUser(fmt, ##__VA_ARGS__)

class Log
{

public:
    enum class e_LogLevel : int 
    {
        Error = 0, Warn, Info
    };

private:    
    e_LogLevel m_LogLevel;
   //m_OutputTarget;

public:
    Log()
        :m_LogLevel(e_LogLevel::Info)
    {
    }

    static Log& Get()
    {
        static Log s_Instance;
        return s_Instance;
    }

    void SetLevel(e_LogLevel level){m_LogLevel = level;}

    using LoggingFunc = void(*)(std::string& msg, const e_LogLevel& LogLevel);

    //This function is called for all WARN, ERROR, INFO messages.
    //It is only for DEBUG mode.
    template <typename... Args>
    inline void LogMessageDebug(LoggingFunc outputFunc, const char* file, const char* func,
            int line, std::string_view fmt,Args&&... args) const
        {
#ifndef NDEBUG
            std::string msg = std::vformat(fmt, std::make_format_args(args...));
            std::string dbg_msg =   msg + " [" + StripPath(file) + " - " + func 
                + " (line " + std::to_string(line) + ")]\n";

            if(outputFunc)
                outputFunc(dbg_msg, m_LogLevel); 
#endif
        }
    //No parameter list (string only) overload of the above function.
    inline void LogMessageDebug(LoggingFunc outputFunc, const char* file, const char* func,
            int line, std::string_view fmt) const
        {
#ifndef NDEBUG
            
            std::string dbg_msg = std::string(fmt) + " [" + StripPath(file) + " - " + func 
                + " (line " + std::to_string(line) + ")]\n";

            if(outputFunc)
                outputFunc(dbg_msg, m_LogLevel); 
#endif
        }

    //This function is called for things we want to print to the console in release mode.
    template <typename... Args>
    inline void LogMessageForUser(std::string_view fmt,Args&&... args) const
    {
        std::string msg = std::vformat(fmt, std::make_format_args(args...));
        std::cout << msg << std::endl;
    }

    //No parameter list (string only) overload of the above function.
    inline void LogMessageForUser(std::string_view fmt) const
    {
        std::cout << std::string(fmt) << std::endl;
    }


    //Debug message functions
    static inline void Error(std::string& msg, const e_LogLevel& LogLevel)
    {
        if (LogLevel >= e_LogLevel::Error)
        { 
            msg = "[ERROR]: " + msg;
            std::cout << msg;
        }
    }

    static inline void Warn(std::string& msg, const e_LogLevel& LogLevel)
    {
        if (LogLevel >= e_LogLevel::Warn)
        {
            msg = "[Warning]: " + msg;
            std::cout << msg;
        }

    }

    static inline void Info(std::string& msg, const e_LogLevel& LogLevel)
    {
        if (LogLevel >= e_LogLevel::Info)
        { 
            msg = "[Info]: " + msg;
            std::cout << msg;
        }
    }

};
