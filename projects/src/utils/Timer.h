/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-23
 * @since version date
 */
#ifndef FINAL_PROJECT_HPC_TIMER_H
#define FINAL_PROJECT_HPC_TIMER_H
#include <chrono>
#include <iostream>
#include <iomanip>

class Timer {

#ifdef TIMERS

private:
    using TimerTimePoint = decltype(std::chrono::high_resolution_clock::now());
    using TimerDuration = decltype(std::chrono::high_resolution_clock::now() -
                                   std::chrono::high_resolution_clock::now());
    static const constexpr std::size_t TIMERS_COUNT = 10;

public:
    static inline void initTimers() {
        for (std::size_t i = 0; i < TIMERS_COUNT; i++) {
            durations[i] = TimerDuration::zero();
        }
    }

    template <std::size_t N>
    static inline void start() {

        static_assert(N < TIMERS_COUNT, "Invalid timer index");
        lastTimers[N] = std::chrono::high_resolution_clock::now();
    }

    template <std::size_t N>
    static inline void stop() {

        static_assert(N < TIMERS_COUNT, "Invalid timer index");

        auto now = std::chrono::high_resolution_clock::now();
        auto newDuration = now - lastTimers[N];
        durations[N] += newDuration;
    }

    template <std::size_t N>
    static inline void print() {

        static_assert(N < TIMERS_COUNT, "Invalid timer index");

        using namespace std::literals::chrono_literals;
        TimerDuration duration = durations[N];

        auto nanoseconds = duration % 1us;
        auto microseconds = (duration % 1ms) / 1us;
        auto milliseconds = (duration % 1s) / 1ms;
        auto seconds = (duration % 1min) / 1s;
        auto minute = duration / 1min;

        std::cout << minute << " minutes " << seconds << " seconds " << milliseconds << " ms (";
        if (milliseconds > 0) {
            std::cout << std::setfill('0') << milliseconds;
            std::cout << ".";
        }
        if (microseconds > 0) {
            int width = (milliseconds > 0) ? 3 : 0;
            std::cout << std::setfill('0') << std::setw(width) << microseconds;
            std::cout << ".";
        }
        int width = (microseconds > 0) ? 3 : 0;
        std::cout << std::setfill('0') << std::setw(width) << nanoseconds.count();
        std::cout << " ns)" << std::endl;
    }
    
    template <typename ...N>
    static inline void printTotal(N... ids) {
        
        
        using namespace std::literals::chrono_literals;
        TimerDuration duration = sum(ids...);

        auto nanoseconds = duration % 1us;
        auto microseconds = (duration % 1ms) / 1us;
        auto milliseconds = (duration % 1s) / 1ms;
        auto seconds = (duration % 1min) / 1s;
        auto minute = duration / 1min;

        std::cout << minute << " minutes " << seconds << " seconds " << milliseconds << " ms (";
        if (milliseconds > 0) {
            std::cout << std::setfill('0') << milliseconds;
            std::cout << ".";
        }
        if (microseconds > 0) {
            int width = (milliseconds > 0) ? 3 : 0;
            std::cout << std::setfill('0') << std::setw(width) << microseconds;
            std::cout << ".";
        }
        int width = (microseconds > 0) ? 3 : 0;
        std::cout << std::setfill('0') << std::setw(width) << nanoseconds.count();
        std::cout << " ns)" << std::endl;
    }

private:
    template <typename N, typename ...Ns>
    static TimerDuration sum(N first, Ns... others) {
        return durations[first] + sum(others...);
    }
    
    template <typename N>
    static TimerDuration sum(N first) {
        return durations[first];
    }
    
    
    static std::array<TimerTimePoint, TIMERS_COUNT> lastTimers;
    static std::array<TimerDuration, TIMERS_COUNT> durations;
#else

public:
    static inline void initTimers() {
    }

    template <std::size_t N>
    static inline void start() {
    }

    template <std::size_t N>
    static inline void stop() {
    }

    template <std::size_t N>
    static inline void print() {
    }
#endif
};
#endif  // FINAL_PROJECT_HPC_TIMER_H
