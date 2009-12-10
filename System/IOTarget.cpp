/*
 *  IOTarget.cpp
 *  System
 *
 *  Created by loch on 9/30/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "IOTarget.h"
#include "Log.h"

IOTarget::IOTarget(): _error(0) {}

IOTarget::~IOTarget() {}

bool IOTarget::atEnd() {
    Warn("atEnd is meaningless on sequential IOTargets");
    return false;
}

bool IOTarget::seek(long long offset, OffsetBase base) {
    Warn("seek is meaningless on sequential IOTargets");
    return false;
}

long long IOTarget::position() {
    Warn("position is meaningless on sequential IOTargets");
    return -1;
}

long long IOTarget::length() {
    Warn("length is meaningless on sequential IOTargets");
    return -1;
}

int IOTarget::error() {
    return _error;
}

void IOTarget::clearError() {
    _error = 0;
}
