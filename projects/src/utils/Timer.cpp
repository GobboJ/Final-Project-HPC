/*
 * Implementation of the timer.
 *
 * @author DeB, Jonathan
 * @version 1.0 2022-07-25
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

void Timer::reset() {
    
    for (std::size_t i = 0; i < TIMERS_COUNT; i++) {
        durations[i] = TimerDuration::zero();
    }
}
}  // namespace cluster::utils
