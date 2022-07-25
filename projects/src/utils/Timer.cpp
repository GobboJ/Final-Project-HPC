/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-25
 * @since version date
 */
#include "Timer.h"

std::array<Timer::TimerTimePoint, Timer::TIMERS_COUNT> Timer::lastTimers{};
std::array<Timer::TimerDuration, Timer::TIMERS_COUNT> Timer::durations{};
