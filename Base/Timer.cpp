/*
 *  Timer.cpp
 *  Mountainhome
 *
 *  Created by loch on 8/21/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "Timer.h"
#include "Logger.h"

Timer::Timer(): _start(0), _elapsed(0), _factor(0.0) {
    mach_timebase_info_data_t info = {0,0}; 
    mach_timebase_info(&info);

    _factor = (double) info.numer / (double) info.denom;
}

void Timer::start() { _start   = mach_absolute_time(); }
void Timer::stop()  { _elapsed = mach_absolute_time() - _start; }

double Timer::seconds() {
    return _elapsed * _factor * 1e-9;
}

double Timer::mseconds() {
    return _elapsed * _factor * 1e-6;
}

double Timer::nseconds() {
    return _elapsed * _factor;
}
