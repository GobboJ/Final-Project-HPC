/*
 * Implementation of the timer.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.1 2022-08-16
 * @since 1.0
 */
#include "../../include/utils/Timer.h"

namespace cluster::utils {
/**
 * Array containing the time at which any timer has been started the last time.
 */
std::array<Timer::TimerTimePoint, Timer::TIMERS_COUNT> Timer::lastTimers{};

/**
 * Array containing the durations held by each timer.
 */
std::array<Timer::TimerDuration, Timer::TIMERS_COUNT> Timer::durations{};

/**
 * Zeroes all the durations of the timers.
 */
void Timer::zeroTimers() {

    // Initialize to 0 all the timers
    for (std::size_t i = 0; i < TIMERS_COUNT; i++) {
        durations[i] = TimerDuration::zero();
    }
}
}  // namespace cluster::utils
