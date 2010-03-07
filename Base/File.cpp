/*
 *  File.cpp
 *  Base
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "Assertion.h"
#include "File.h"

#include "FileSystem.h"

File::File(const std::string &dir, const std::string &file)
: _dirName(dir), _fileName(file), _fullName(""), _mode(None) {
    _fullName = _dirName + _fileName;
}

File::File(const std::string &dir, const std::string &file, OpenMode openFlags)
: _dirName(dir), _fileName(file), _fullName(""), _mode(None) {
    _fullName = _dirName + _fileName;
    if (openFlags) {
        open(openFlags);
    }
}

File::~File() {
    if (isOpen()) {
        close();
    }
}

long long File::bytesLeft() {
    return length() - position();
}

char File::peek() {
    if (!(_mode & Read)) {
        Warn("Attempted to read a file not opened for reading: " << _fullName);
        return 0;
    }

    return _internal.peek();
}

char File::getc() {
    if (!(_mode & Read)) {
        Warn("Attempted to read a file not opened for reading: " << _fullName);
        return 0;
    }

    return _internal.get();
}

long long File::read(void* buffer, long long size) {
    if (!(_mode & Read)) {
        Warn("Attempted to read a file not opened for reading: " << _fullName);
        return 0;
    }

    long long originalPos = position();
    _internal.read((char*)buffer, size);
    return position() - originalPos;
}

long long File::write(const void* buffer, long long size) {
    if (!(_mode & Write)) {
        Warn("Attempted to write to a file not opened for writing: " << _fullName);
        return 0;
    }

    long long originalPos = position();
    _internal.write((char*)buffer, size);
    return position() - originalPos;
}

int File::error() {
    return _internal.rdstate();
}

void File::clearError() {
    _internal.clear();
}

bool File::seek(long long offset, OffsetBase base) {
    long long oldPosition = position();
    long long endPosition = 0;

    switch(base) {
        case Current:
            endPosition = oldPosition + offset;
            break;
        case Beginning:
            endPosition = offset;
            break;
        case End:
            endPosition = length() + offset;
            break;
        default:
            Error("Unknown base given to seek. File name: " << _fullName);
    }

    if (endPosition > length()) {
        Warn("Tried to seek to " << endPosition << " in " << _fullName);
        endPosition = length();
        Warn("  Seeking to " << endPosition << " instead");
    }

    if (endPosition < 0) {
        Warn("Tried to seek to " << endPosition << " in " << _fullName);
        endPosition = 0;
        Warn("  Seeking to " << endPosition << " instead");
    }

    _internal.seekp(endPosition);

    if (error()) {
        Warn("Error seeking in file: " << _fullName);
        _internal.clear();
        _internal.seekp(oldPosition);
        return false;
    }

    return true;
}

long long File::position() {
    ASSERT_EQ(_internal.tellp(), _internal.tellg());
    return _internal.tellp();
}

bool File::open(OpenMode openFlags) {
    if (isOpen()) {
        Warn("File " << _fullName << " is already open with flags: " << _mode);
        Warn("  Closing file and reopening with flags: " << openFlags);
        close();
    }

    if (!(openFlags & Write) && !exists()) {
        Warn("File does not exist to open as read only: " << _fullName);
        return false;
    }

    if (openFlags == 0) {
        Warn("Attempting to open file without open flags: " << _fullName);
        return false;
    }

    _mode = openFlags;
    std::ios_base::openmode mode = std::fstream::binary;
    mode |= (_mode & Read)   ? std::fstream::in  : mode;
    mode |= (_mode & Write)  ? std::fstream::out : mode;
    mode |= (_mode & Append) ? std::fstream::app : mode;

    // If the file is an in and out file but does not exist, the file will fail to open.
    // To work around this we simply touch the file before we try to open it.
    if (_mode & Read && _mode & Write && !exists()) {
        FileSystem::Touch(_fullName);
    }

    _internal.open(_fullName.c_str(), mode);

    if (!isOpen()) {
        Warn("File failed to open: " << _fullName);
        Warn("  Open flags:  " << _mode);
        Warn("  File exists: " << exists());
        Warn("  Error flag:  " << error());
        return false;
    }

    return true;
}

bool File::atEnd() {
    return _internal.eof();
}

bool File::deleteFile() {
    if (isOpen()) {
        Warn("Trying to delete an opened file: " << _fullName);
        return false;
    }

    return FileSystem::Delete(_fullName);
}

bool File::exists() {
    if (isOpen()) { return true; }
    return FileSystem::Exists(_fullName);
}

void File::close() {
    if (isOpen()) {
        _internal.close();
    } else {
        Warn("Trying to close a file that is not open: " << _fullName);
    }
}

long long File::length() {
    return FileSystem::Length(_fullName);
}

bool File::isOpen() {
    return _internal.is_open();
}

const string& File::fullName() {
    return _fullName;
}

const string& File::dirName() {
    return _dirName;
}

const string& File::fileName() {
    return _fileName;
}
