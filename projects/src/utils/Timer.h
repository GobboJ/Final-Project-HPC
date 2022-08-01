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
#include <array>

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
    static void print() {

        static_assert(N < TIMERS_COUNT, "Invalid timer index");

        printDuration(durations[N]);
    }

    template <std::size_t... Ns>
    static void printTotal() {

        printDuration(sum<Ns...>());
    }

private:
    static void printDuration(TimerDuration duration) {

        using namespace std::literals::chrono_literals;

        auto nanoseconds = duration % 1us;
        auto microseconds = (duration % 1ms) / 1us;
        auto milliseconds = (duration % 1s) / 1ms;
        auto seconds = (duration % 1min) / 1s;
        auto minute = duration / 1min;

        std::cout << minute << " minutes " << std::setw(2) << std::setfill(' ') << seconds
                  << " seconds " << std::setw(3) << std::setfill(' ') << milliseconds << " ms (";
        if (milliseconds > 0) {
            std::cout << std::setw(3) << std::setfill(' ') << milliseconds;
            std::cout << ".";
        } else {
            std::cout << "    ";
        }
        if (microseconds > 0) {
            std::cout << std::setw(3) << std::setfill(' ') << microseconds;
            std::cout << ".";
        } else {
            std::cout << "    ";
        }
        if (microseconds > 0) {
            std::cout << std::setw(3) << std::setfill('0') << nanoseconds.count();
        } else {
            std::cout << std::setw(3) << std::setfill(' ') << nanoseconds.count();
        }
        std::cout << " ns)\033[K" << std::endl;
    }

    template <std::size_t N, std::size_t... Ns>
    static TimerDuration sum() {
        if constexpr (sizeof...(Ns) == 1) {
            return durations[N];
        } else {
            return durations[N] + sum<Ns...>();
        }
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

    template <typename... N>
    static void printTotal(N... ids) {
    }
#endif
};
#endif  // FINAL_PROJECT_HPC_TIMER_H
