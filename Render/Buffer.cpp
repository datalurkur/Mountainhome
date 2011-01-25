/*
 *  Buffer.cpp
 *  Mountainhome
 *
 *  Created by loch on 12/17/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "Buffer.h"

Buffer::Buffer(
    GLenum bufferType,
    GLenum accessType,
    GLenum dataType,
    unsigned int elementCount,
    void *data
):
    _bufferType(bufferType),
    _accessType(accessType),
    _dataType(dataType),
    _elementCount(elementCount),
    _data(data),
    _handle(0),
    _byteCount(0)
{}

Buffer::~Buffer() {}

GLenum Buffer::getAccessType() {
    return _accessType;
}

GLenum Buffer::getDataType() {
    return _dataType;
}

unsigned int Buffer::getElementCount() {
    return _elementCount;
}

unsigned int Buffer::getByteCount() {
    return _byteCount;
}
