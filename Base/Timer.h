/*
 *  Timer.h
 *  Mountainhome
 *
 *  Created by loch on 8/21/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _TIMER_H_
#define _TIMER_H_
#include <mach/mach_time.h>

class Timer {
public:
    Timer();
    void start();
    void stop();

    double seconds();
    double mseconds();
    double nseconds();

private:
    uint64_t _start, _elapsed;
    double _factor;

};

#endif
