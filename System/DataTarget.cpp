/*
 *  DataTarget.cpp
 *  Base
 *
 *  Created by Brent Wilson on 11/30/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "DataTarget.h"
#include "Math3D.h"

#include <memory>

DataTarget::DataTarget(unsigned char *data, int length): _data(data), _length(length),
_pos(0) {}

DataTarget::~DataTarget() {
    delete[] _data;
}

bool DataTarget::open(OpenMode openFlags) {
    return true;
}
void DataTarget::close() {}

bool DataTarget::isOpen() {
    return true;
}

int DataTarget::error() {
    return 0;
}
void DataTarget::clearError() {}

long long DataTarget::bytesLeft() {
    return _length - _pos;
}
char DataTarget::peek() {
    return _data[_pos];
}
char DataTarget::getc() {
    char result = peek();
    _pos++;
    return result;
}

bool DataTarget::atEnd() {
    return _pos == _length;
}

long long DataTarget::position() {
    return _pos;
}

long long DataTarget::length() {
    return _length;
}

long long DataTarget::read(void* buffer, long long size) {
    long long toRead = Math::Min(size, bytesLeft());
    memcpy(buffer, _data + _pos, toRead);
    _pos += toRead;
    return toRead;
}

long long DataTarget::write(const void* buffer, long long size) {
    long long toRead = Math::Min(size, bytesLeft());
    memcpy(_data + _pos, buffer, toRead);
    _pos += toRead;
    return toRead;
}

bool DataTarget::seek(long long offset, OffsetBase base) {
    switch(base) {
        case IOTarget::Beginning:
            _pos = 0;
            break;
        case IOTarget::End:
            _pos = _length;
            break;
    }

    _pos += offset;
    if (_pos > length()) {
        Warn("Tried to seek to " << _pos);
        _pos = length();
        Warn("  Seeking to " << _pos << " instead");
    }

    if (_pos < 0) {
        Warn("Tried to seek to " << _pos);
        _pos = 0;
        Warn("  Seeking to " << _pos << " instead");
    }

    return true;
}
