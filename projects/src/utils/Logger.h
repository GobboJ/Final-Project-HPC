#ifndef FINAL_PROJECT_HPC_LOGGER_H
#define FINAL_PROJECT_HPC_LOGGER_H

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-31
 * @since version date
 */
#include <cstddef>
#include <iostream>
#include "Timer.h"

class Logger {

public:
#ifdef PRINT_ITERATIONS
    template <std::size_t... Ns>
    static void startLoggingProgress(std::size_t dataSize) {

        std::cout << "Processed 0 / " << dataSize << " rows" << std::endl;
        printStages<1, Ns...>();
        std::cout << "Total  : ";
        Timer::printTotal<Ns...>();
        std::cout.flush();
    }

    template <std::size_t I, std::size_t... Ns>
    static void logProgress(std::size_t currentProgress, std::size_t dataSize) {

        if (currentProgress % I == 0) {
            std::cout << "\033[" << sizeof...(Ns) + 2 << "AProcessed " << currentProgress << " / "
                      << dataSize << " rows\033[K" << std::endl;
            printStages<1, Ns...>();
            std::cout << "Total  : ";
            Timer::printTotal<Ns...>();
        }
    }

private:
    template <std::size_t S, std::size_t N, std::size_t... Ns>
    static inline void printStages() {

        printStage<S, N>();
        if constexpr (sizeof...(Ns) >= 1) {
            printStages<S + 1, Ns...>();
        }
    }

    template <std::size_t S, std::size_t N>
    static inline void printStage() {

        std::cout << "Stage" << ' ' << S << ": ";
        Timer::print<N>();
    }

#else
    template <std::size_t... Ns>
    static inline void startLoggingProgress(std::size_t dataSize) {
    }

    template <std::size_t I, std::size_t... Ns>
    static inline void logProgress(std::size_t currentProgress, std::size_t dataSize) {
    }
#endif
};

#endif  // FINAL_PROJECT_HPC_LOGGER_H
