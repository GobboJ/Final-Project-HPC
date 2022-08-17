#ifndef FINAL_PROJECT_HPC_TIMER_H
#define FINAL_PROJECT_HPC_TIMER_H
#include <array>
#include <chrono>
#include <iomanip>
#include <iostream>

namespace cluster::utils {

/**
 * Utility class allowing to measure the time needed to execute a block of code. To use this class,
 * just invoke the <code>Timer::start</code> method before the first line of the block of code to
 * measure, and then invoke the <code>Timer::stop</code> right after the last line of the block of
 * code to measure.<br>
 * For example:
 * <pre>
 * Timer::start&lt;0&gt;();<br>
 * // Code to measure<br>
 * Timer::stop&lt;0&gt;();
 * </pre>
 * <br>
 * This class allows to use up to 10 timers, numbered from <code>0</code> to <code>9</code>.
 * However, before using them, remember to zero them invoking the
 * <code>Timer::zeroTimers</code> method.<br>
 * The timers can be enabled or disabled by using the <code>TIMERS</code> macro. In particular, if
 * it is not defined, then any invocation to any of the methods provided by this class has no
 * effect, hence the timers are disabled.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.1 2022-08-16
 * @since 1.0
 */
class Timer {

#ifdef TIMERS

private:
    /**
     * Type of the time each timer has been started the last time.
     */
    using TimerTimePoint = decltype(std::chrono::high_resolution_clock::now());

    /**
     * Type of the duration of the timers.
     */
    using TimerDuration = decltype(std::chrono::high_resolution_clock::now() -
                                   std::chrono::high_resolution_clock::now());

    /**
     * Number of available timers.
     */
    static const constexpr std::size_t TIMERS_COUNT = 10;

public:
    /**
     * Utility method that zeroes all the 10 timers. This method must be called before any
     * timer is used.
     */
    static void zeroTimers();

    /**
     * Starts or resumes the timer with the specified identifier.
     *
     * @tparam N Identifier of the timer to start or resume.
     */
    template <std::size_t N>
    static inline void start() {

        // Check the validity of the identifier of the timer
        static_assert(N < TIMERS_COUNT, "Invalid timer index");

        // Update the last time the timer was started
        lastTimers[N] = std::chrono::high_resolution_clock::now();
    }

    /**
     * Stops the timer with the specified identifier.
     *
     * @tparam N Identifier of the timer to stop.
     */
    template <std::size_t N>
    static inline void stop() {

        // Check the validity of the identifier of the timer
        static_assert(N < TIMERS_COUNT, "Invalid timer index");

        // Add to the duration of the timer the time passed from when the timer has started to now
        auto now = std::chrono::high_resolution_clock::now();
        auto newDuration = now - lastTimers[N];
        durations[N] += newDuration;
    }

    /**
     * Prints to the console, in a human-readable format, the duration held by the timer with the
     * specified identifier.<br>
     * This method allows also to compute the mean execution time of the block(s) of code enclosed
     * between the <code>Timer::start</code> and <code>Timer::stop</code> invocations of the timer
     * with the specified identifier.
     *
     * @tparam N Identifier of the timer to print.
     * @param times The number of times the block(s) of code enclosed between the timer invocations
     * is executed. This value is used to divide the total timer duration so to compute the mean
     * execution time of the block(s) of code.
     */
    template <std::size_t N>
    static void print(std::size_t times = 1) {

        // Check the validity of the identifier of the timer
        static_assert(N < TIMERS_COUNT, "Invalid timer index");

        // Print the duration of the timer
        printDuration(durations[N] / times);
    }

    /**
     * Prints to the console, in a human-readable format, the sum of the durations held by all the
     * specified timers. If no identifier is specified, then the <code>0</code> duration is
     * printed.<br>
     * This method allows also to compute the mean execution time of all the blocks of code enclosed
     * between the <code>Timer::start</code> and <code>Timer::stop</code> invocations of all the
     * timers with the specified identifiers.
     *
     * @tparam Ns Identifiers of the timers whose sum of the durations is printed.
     * @param times The number of times the blocks of code enclosed between the invocations of
     * timers are executed. This value is used to divide the total timer durations so to compute the
     * mean execution time of the blocks of code.
     */
    template <std::size_t... Ns>
    static void printTotal(std::size_t times = 1) {

        // Sum the durations and print the result
        printDuration(sum<Ns...>() / times);
    }

private:
    /**
     * Prints to the console the duration of a timer in a human-readable form.
     *
     * @param duration Duration to print.
     */
    static void printDuration(TimerDuration duration) {

        // Use the literals for the durations, so to make the code cleaner
        using namespace std::literals::chrono_literals;

        // Extract each part of the duration
        auto nanoseconds = duration % 1us;
        auto microseconds = (duration % 1ms) / 1us;
        auto milliseconds = (duration % 1s) / 1ms;
        auto seconds = (duration % 1min) / 1s;
        auto minute = duration / 1min;

        // Print minutes, seconds and milliseconds
        std::cout << minute << " minutes " << std::setw(2) << std::setfill(' ') << seconds
                  << " seconds " << std::setw(3) << std::setfill(' ') << milliseconds << " ms (";

        // Print the milliseconds, properly padded
        if (milliseconds > 0) {
            std::cout << std::setw(3) << std::setfill(' ') << milliseconds;
            std::cout << ".";
        } else {
            std::cout << "    ";
        }
        // Print the microseconds, properly padded
        if (milliseconds > 0 || microseconds > 0) {
            char fill = (milliseconds > 0) ? '0' : ' ';
            std::cout << std::setw(3) << std::setfill(fill) << microseconds;
            std::cout << ".";
        } else {
            std::cout << "    ";
        }
        // Print the nanoseconds, properly padded
        char fill = (microseconds > 0) ? '0' : ' ';
        std::cout << std::setw(3) << std::setfill(fill) << nanoseconds.count();

        // Clear the remaining part of the line
        std::cout << " ns)\033[K" << std::endl;
    }

    /**
     * Utility method that recursively sums the durations of the specified timers.
     *
     * @tparam N Identifier of the timer whose duration is to be summed with the durations of the
     * other timers.
     * @tparam Ns Identifiers of the other timers whose duration is to be summed to the duration of
     * the specified timer. If no identifier is specified, then only the duration of the timer with
     * identifier <code>N</code> is returned.
     * @return The sum of the durations of the timers.
     */
    template <std::size_t N, std::size_t... Ns>
    static TimerDuration sum() {

        // Check the validity of the identifier of the timer
        static_assert(N < TIMERS_COUNT, "Invalid timer index");

        if constexpr (sizeof...(Ns) == 0) {
            return durations[N];
        } else {
            return durations[N] + sum<Ns...>();
        }
    }

    /**
     * Array containing the time at which any timer has been started the last time.
     */
    static std::array<TimerTimePoint, TIMERS_COUNT> lastTimers;

    /**
     * Array containing the durations held by each timer.
     */
    static std::array<TimerDuration, TIMERS_COUNT> durations;
#else

public:
    /**
     * Does nothing because the <code>TIMERS</code> macro is not defined.
     */
    static inline void zeroTimers() {
    }

    /**
     * Does nothing because the <code>TIMERS</code> macro is not defined.
     */
    template <std::size_t>
    static inline void start() {
    }

    /**
     * Does nothing because the <code>TIMERS</code> macro is not defined.
     */
    template <std::size_t>
    static inline void stop() {
    }

    /**
     * Does nothing because the <code>TIMERS</code> macro is not defined.
     *
     * @param times Ignored.
     */
    template <std::size_t>
    static inline void print([[maybe_unused]] std::size_t times = 1) {
    }

    /**
     * Does nothing because the <code>TIMERS</code> macro is not defined.
     *
     * @tparam ... Ignored.
     * @param times Ignored.
     */
    template <std::size_t...>
    static inline void printTotal([[maybe_unused]] std::size_t times = 1) {
    }
#endif
};
}  // namespace cluster::utils
#endif  // FINAL_PROJECT_HPC_TIMER_H
