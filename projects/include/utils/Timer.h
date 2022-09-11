#ifndef FINAL_PROJECT_HPC_TIMER_H
#define FINAL_PROJECT_HPC_TIMER_H
#include <array>
#include <chrono>
#include <fstream>
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
 * @version 1.2 2022-09-11
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
     * Prints to the specified output stream the duration held by the timer with the specified
     * identifier.<br>
     * If the template parameter <code>S</code> is <code>true</code>, then this method will print to
     * the specified output stream a simplified output. In particular, this method will only print
     * the duration, in nanoseconds, held by the timer with the specified identifier.<br>
     * If, instead, the template parameter <code>S</code> is <code>false</code>, then this method
     * will print to the specified output stream the duration held by the timer with the specified
     * identifier in a human-readable format.
     *
     * @tparam N Identifier of the timer to print.
     * @tparam S <code>true</code> if this method should print a simplified output to the specified
     * output stream, <code>false</code> otherwise.
     * @param outputStream Stream where the duration of the timer will be printed to.
     */
    template <std::size_t N, bool S = false>
    static void print(std::ostream &outputStream = std::cout) {

        // Check the validity of the identifier of the timer
        static_assert(N < TIMERS_COUNT, "Invalid timer index");

        // Print the duration of the timer
        printDuration<S>(outputStream, durations[N]);
    }

    /**
     * Prints to the specified output stream the sum of the durations held by all the timers with
     * the specified identifiers. If no identifier is specified, then the <code>0</code> duration is
     * printed.<br>
     * If the template parameter <code>S</code> is <code>true</code>, then this method will print to
     * the specified output stream a simplified output. In particular, this method will only print
     * the sum of the duration of the timers in nanoseconds.<br>
     * If, instead, the template parameter <code>S</code> is <code>false</code>, then this method
     * will print to the specified output stream the sum of the duration of the timers in a
     * human-readable format.
     *
     * @tparam Ns Identifiers of the timers whose sum of the durations is printed.
     * @tparam S <code>true</code> if this method should print a simplified output to the specified
     * output stream, <code>false</code> otherwise.
     * @param outputStream Stream where the sum of the duration of the timers will be printed to.
     */
    template <bool S, std::size_t... Ns>
    static void printTotal(std::ostream &outputStream = std::cout) {

        // Sum the durations and print the result
        printDuration<S>(outputStream, sum<Ns...>());
    }

private:
    /**
     * Prints to the specified output stream the duration of a timer in a simplified or
     * human-readable format.
     *
     * @tparam S Whether this method should print a simplified output.
     * @param outputStream Stream where the duration of the timer will be printed to.
     * @param duration Duration to print.
     */
    template <bool S>
    static void printDuration(std::ostream &outputStream, const TimerDuration duration) {

        // Use the literals for the durations, so to make the code cleaner
        using namespace std::literals::chrono_literals;

        // CHeck if the simplified output has been requested
        if constexpr (S) {
            // Print just the nanoseconds
            outputStream << (duration / 1ns) << std::endl;
        } else {
            // Extract each part of the duration
            const auto nanoseconds = duration % 1us;
            const auto microseconds = (duration % 1ms) / 1us;
            const auto milliseconds = (duration % 1s) / 1ms;
            const auto seconds = (duration % 1min) / 1s;
            const auto minute = duration / 1min;

            // Print minutes, seconds and milliseconds
            outputStream << minute << " minutes " << std::setw(2) << std::setfill(' ') << seconds
                         << " seconds " << std::setw(3) << std::setfill(' ') << milliseconds
                         << " ms (";

            // Print the milliseconds, properly padded
            if (milliseconds > 0) {
                outputStream << std::setw(3) << std::setfill(' ') << milliseconds;
                outputStream << ".";
            } else {
                outputStream << "    ";
            }
            // Print the microseconds, properly padded
            if (milliseconds > 0 || microseconds > 0) {
                const char fill = (milliseconds > 0) ? '0' : ' ';
                outputStream << std::setw(3) << std::setfill(fill) << microseconds;
                outputStream << ".";
            } else {
                outputStream << "    ";
            }
            // Print the nanoseconds, properly padded
            const char fill = (microseconds > 0) ? '0' : ' ';
            outputStream << std::setw(3) << std::setfill(fill) << nanoseconds.count();

            // Clear the remaining part of the line
            outputStream << " ns)\033[K" << std::endl;
        }
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
     * @param outputStream Ignored.
     */
    template <std::size_t, bool = false>
    static inline void print([[maybe_unused]] std::ostream &outputStream = std::cout) {
    }

    /**
     * Does nothing because the <code>TIMERS</code> macro is not defined.
     *
     * @tparam ... Ignored.
     * @param outputStream Ignored.
     */
    template <bool, std::size_t...>
    static inline void printTotal([[maybe_unused]] std::ostream &outputStream = std::cout) {
    }
#endif
};
}  // namespace cluster::utils
#endif  // FINAL_PROJECT_HPC_TIMER_H
