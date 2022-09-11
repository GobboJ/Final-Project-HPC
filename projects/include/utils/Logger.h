#ifndef FINAL_PROJECT_HPC_LOGGER_H
#define FINAL_PROJECT_HPC_LOGGER_H

#include "Timer.h"
#include <cstddef>
#include <iostream>

namespace cluster::utils {

/**
 * Utility class allowing to log to the console the progress of the application.<br>
 * The logging can be enabled or disabled by using the <code>PRINT_ITERATIONS</code> macro. In
 * particular, if it is not defined, then any invocation to any of the methods provided by this
 * class has no effect, hence the logging is disabled.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.0.3 2022-09-11
 * @since 1.0
 */
class Logger {

public:
#ifdef PRINT_ITERATIONS
    /**
     * Starts logging the progress.
     *
     * @tparam Ns Identifiers of the timer to print together with the progress. If no identifier is
     * specified, then no timer is printed.
     * @param workSize Size, in elements, of the total work to do.
     */
    template <std::size_t... Ns>
    static void startLoggingProgress(const std::size_t workSize) {

        // Print the initial progress
        std::cout << "Processed 0 /" << ' ' << workSize << " rows" << std::endl;
        // Print the timers, if any
        if constexpr (sizeof...(Ns) > 0) {
            printStages<0, Ns...>();
            // Print the total time
            std::cout << "Total  : ";
            Timer::printTotal<false, Ns...>();
        }
    }

    /**
     * Updates the progress printed to the console, overwriting the previous progress. To control
     * how many times the progress is updated, this method updates the progress on the console only
     * when the <code>currentProgress</code> is a multiple of <code>I</code>.
     *
     * @tparam I Non-zero number that, when <code>currentProgress</code> is a multiple of it,
     * allows this method to update the progress on the console. If this number is <code>1</code>,
     * then the progress is logged on the console every time this method is invoked.
     * @tparam Ns Identifiers of the timer to print together with the progress. If no identifier is
     * specified, then no timer is printed.
     * @param currentProgress Number of elements being processed.
     * @param workSize Size, in elements, of the total work to do.
     */
    template <std::size_t I, std::size_t... Ns>
    static void updateProgress(const std::size_t currentProgress, const std::size_t workSize) {

        // Require I to be non-zero
        static_assert(I != 0, "The value for 'I' cannot be 0");

        // Check if the progress should be updates
        if (currentProgress % I == 0) {
            std::cout <<
                    // Come back to the first line of the progress
                    "\033[" << sizeof...(Ns) + 2 << 'A' <<
                    // Print the current progress
                    "Processed" << ' ' << currentProgress << " / " << workSize <<
                    // Clear the remaining part of the line
                    " rows\033[K" << std::endl;

            // Print the timers, if any
            if constexpr (sizeof...(Ns) > 0) {
                printStages<0, Ns...>();
                // Print the total time
                std::cout << "Total  : ";
                Timer::printTotal<false, Ns...>();
            }
        }
    }

private:
    /**
     * Utility method that prints all the timers with identifier greater than or equal to
     * <code>N</code>.
     *
     * @tparam S Number identifying the stage the timer is related to. <code>0</code> identifies the
     * initialization stage.
     * @tparam N Identifier of the timer to print.
     * @tparam Ns Identifiers of the remaining timers to print.
     */
    template <std::size_t S, std::size_t N, std::size_t... Ns>
    static inline void printStages() {

        // Print the times
        printStage<S, N>();
        // Print the remaining timers, if any
        if constexpr (sizeof...(Ns) >= 1) {
            printStages<S + 1, Ns...>();
        }
    }

    /**
     * Prints the timer with identifier <code>N</code> associated to the stage <code>S</code>.
     *
     * @tparam S Number of the stage, where <code>0</code> identifies the initialization stage.
     * @tparam N Identifier of the timer to print.
     */
    template <std::size_t S, std::size_t N>
    static inline void printStage() {

        // Print a text informing of what stage the timer is related to
        if constexpr (S == 0) {
            std::cout << "Service: ";
        } else {
            std::cout << "Stage" << ' ' << S << ": ";
        }
        // Print the timer
        Timer::print<N>();
    }

#else
    /**
     * Does nothing because the <code>PRINT_ITERATIONS</code> macro is not defined.
     *
     * @tparam ... Ignored.
     * @param workSize Ignored.
     */
    template <std::size_t...>
    static inline void startLoggingProgress([[maybe_unused]] const std::size_t workSize) {
    }

    /**
     * Does nothing because the <code>PRINT_ITERATIONS</code> macro is not defined.
     *
     * @tparam ... Ignored.
     * @param currentProgress Ignored.
     * @param workSize Ignored.
     */
    template <std::size_t, std::size_t...>
    static inline void updateProgress([[maybe_unused]] const std::size_t currentProgress,
                                      [[maybe_unused]] const std::size_t workSize) {
    }
#endif
};
}  // namespace cluster::utils
#endif  // FINAL_PROJECT_HPC_LOGGER_H
