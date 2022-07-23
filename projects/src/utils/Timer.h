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

private:
    Timer(decltype(std::chrono::high_resolution_clock::now() -
                   std::chrono::high_resolution_clock::now()) duration) :
        duration(duration) {
    }

public:
    Timer() : duration(0) {
    }

    void start() {
        lastTimer = std::chrono::high_resolution_clock::now();
    }

    void stop() {

        auto now = std::chrono::high_resolution_clock::now();
        auto newDuration = now - lastTimer;
        this->duration += newDuration;
    }

    void print() {

        using namespace std::literals::chrono_literals;

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

    Timer operator+ (Timer &second) {

        return {this->duration + second.duration};
    }

private:
    decltype(std::chrono::high_resolution_clock::now()) lastTimer;
    decltype(std::chrono::high_resolution_clock::now() -
             std::chrono::high_resolution_clock::now()) duration;
};
#endif  // FINAL_PROJECT_HPC_TIMER_H
