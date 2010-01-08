/*
 *  TextStream.cpp
 *  Base
 *
 *  Created by Brent Wilson on 9/30/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "TextStream.h"

TextStream::TextStream(IOTarget *target, IOTarget::OpenMode mode, bool cleanup)
: _target(target), _delimeter(" \n"), _cleanup(cleanup) {
    if (_target && mode) {
        _target->open(mode);
    }

    if (!_target) {
        _cleanup = false;
    }
}

TextStream::~TextStream() {
    if (_cleanup) {
        if (_target->isOpen()) {
            _target->close();
        }

        delete _target;
        _target = NULL;
    }
}

long long TextStream::bytesLeft() {
    return _target->bytesLeft();
}

bool TextStream::isValid() {
    return _target && _target->isOpen() && !_target->error();
}

bool TextStream::isDelimeter(char check) {
    return _delimeter.find(check, 0) != std::string::npos;
}

void TextStream::setDelimeter(const std::string &delimeter) {
    _delimeter = delimeter;
}

bool TextStream::seek(long long offset, IOTarget::OffsetBase base) {
    return _target->seek(offset, base);
}

long long TextStream::position() {
    return _target->position();
}

bool TextStream::atEnd() {
    return _target->atEnd();
}

long long TextStream::write(const char* pointer, long long size) {
    return _target->write(pointer, size);
}

long long TextStream::write(const std::string &buffer) {
    return write(buffer.c_str(), buffer.length());
}

long long TextStream::read(char* pointer, long long size) {
    return _target->read(pointer, size);
}

long long TextStream::readAll(std::string &result) {
    if (!_target->isOpen()) { return 0; }

    std::string temp;
    while (!atEnd()) {
        readLine(temp);
        result += temp + (atEnd() ? "" : "\n");
        if (_target->error()) {
            break;
        }
    }

    return result.length();
}

long long TextStream::readLine(std::string &result) {
    if (!_target->isOpen()) { return 0; }
    std::string temp = _delimeter;

    _delimeter = "\n";
    long long bytesRead = readChunk(result);
    _delimeter = temp;

    return bytesRead;
}

long long TextStream::readChunk(std::string &result) {
    bool readNonDelimeter = false;
    char current = 0;
    result = "";

    while ((!isDelimeter(_target->peek()) || !readNonDelimeter) && !atEnd()) {
        current = _target->getc();
        if (_target->error()) {
            break;
        }

        if (!isDelimeter(current)) {
            result += current;
            readNonDelimeter = true;
        }
    }

    return result.length();
}

long long TextStream::read(std::string &result, long long size) {
    if (!_target->isOpen()) { return 0; }

    char *buffer = new char(size);
    read(buffer, size);

    result = std::string(buffer);
    delete buffer;

    return result.length();
}

TextStream& TextStream::operator>>(float &f) {
    std::string result;
    readChunk(result);
    f = atof(result.c_str());

    return (*this);
}

TextStream& TextStream::operator>>(double &f) {
    std::string result;
    readChunk(result);
    f = atof(result.c_str());

    return (*this);
}

TextStream& TextStream::operator>>(std::string &str) {
    readChunk(str);
    return (*this);
}

TextStream& TextStream::operator>>(char &c) {
    std::string result;
    readChunk(result);
    c = atoi(result.c_str());

    return (*this);
}

TextStream& TextStream::operator>>(signed short &i) {
    std::string result;
    readChunk(result);
    i = atoi(result.c_str());

    return (*this);
}

TextStream& TextStream::operator>>(unsigned short &i) {
    std::string result;
    readChunk(result);
    i = atoi(result.c_str());

    return (*this);
}

TextStream& TextStream::operator>>(signed int &i) {
    std::string result;
    readChunk(result);
    i = atoi(result.c_str());

    return (*this);
}

TextStream& TextStream::operator>>(unsigned int &i) {
    std::string result;
    readChunk(result);
    i = atoi(result.c_str());

    return (*this);
}

TextStream& TextStream::operator>>(signed long &i) {
    std::string result;
    readChunk(result);
    i = atoi(result.c_str());

    return (*this);
}

TextStream& TextStream::operator>>(unsigned long &i) {
    std::string result;
    readChunk(result);
    i = atoi(result.c_str());

    return (*this);
}
