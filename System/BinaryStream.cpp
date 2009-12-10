/*
 *  BinaryStream.cpp
 *  System
 *
 *  Created by loch on 9/30/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "BinaryStream.h"

BinaryStream::BinaryStream(IOTarget *target, IOTarget::OpenMode mode, bool cleanup)
: _target(target), _cleanup(cleanup) {
    if (_target && mode) {
        _target->open(mode);
    }

    if (!_target) {
        _cleanup = false;
    }
}

BinaryStream::~BinaryStream() {
    if (_cleanup) {
        if (_target->isOpen()) {
            _target->close();
        }

        delete _target;
        _target = NULL;
    }
}

long long BinaryStream::bytesLeft() {
    return _target->bytesLeft();
}

bool BinaryStream::isValid() {
    return _target && _target->isOpen() && !_target->error();
}

bool BinaryStream::seek(long long offset, IOTarget::OffsetBase base) {
    return _target->seek(offset, base);
}

long long BinaryStream::position() {
    return _target->position();
}

long long BinaryStream::length() {
    return _target->length();
}

bool BinaryStream::atEnd() {
    return _target->atEnd();
}

long long BinaryStream::write(const std::string &buffer) {
    return write(buffer.c_str(), buffer.length());
}

long long BinaryStream::write(const void* pointer, long long size) {
    return _target->write(pointer, size);
}

long long BinaryStream::read(void* pointer, long long size) {
    return _target->read(pointer, size);
}
