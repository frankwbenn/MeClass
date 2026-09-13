#pragma once
#include <chrono>
#include <string>
#include "log.h"

class ScopeTimer {
using Clock = std::chrono::high_resolution_clock;

private:
    std::string m_Name;
    Clock::time_point m_StartTime;    

public:

    template <typename... Args>
    ScopeTimer(std::string name) 
        : m_Name(std::move(name)), m_StartTime(Clock::now())
    {
    }

    ~ScopeTimer() {
        using namespace std::chrono;
        auto endTime = Clock::now();
        auto duration = duration_cast<microseconds>(endTime - m_StartTime).count();
        double duration_ms = static_cast<double>(duration) / 1000.0;
        // Automatically logs when the timer goes out of scope 
        LOG_INFO("Benchmark [{}] took {} µs ({} ms)", m_Name, duration, duration_ms);
    }
};

#define PROFILE_SCOPE() ScopeTimer timer_##__LINE___{__builtin_FUNCTION()}
#define PROFILE_NAMED(name) ScopeTimer timer_##__LINE___{name}
