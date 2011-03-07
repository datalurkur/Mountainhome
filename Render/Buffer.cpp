/*
 *  Buffer.cpp
 *  Mountainhome
 *
 *  Created by loch on 12/17/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "Buffer.h"

#include <Base/Assertion.h>

Buffer::Buffer(
    GLenum bufferType,
    GLenum accessType,
    GLenum dataType,
    unsigned int elementSize,
    unsigned int elementCount,
    void *data
):
    _bufferType(bufferType),
    _accessType(accessType),
    _dataType(dataType),
    _elementSize(elementSize),
    _elementCount(elementCount),
    _data(data),
    _handle(0),
    _byteCount(0)
{
    int sizeOfComponent = 0;
    
    switch(_dataType) {
        case GL_BYTE:
            // Don't use signed indices. No reason for it.
            ASSERT(_bufferType != GL_ELEMENT_ARRAY_BUFFER);
        case GL_UNSIGNED_BYTE:
            sizeOfComponent = sizeof(char);
            break;
        case GL_SHORT:
            // Don't use signed indices. No reason for it.
            ASSERT(_bufferType != GL_ELEMENT_ARRAY_BUFFER);
        case GL_UNSIGNED_SHORT:
            sizeOfComponent = sizeof(short);
            break;
        case GL_INT:
            // Don't use signed indices. No reason for it.
            ASSERT(_bufferType != GL_ELEMENT_ARRAY_BUFFER);
        case GL_UNSIGNED_INT:
            sizeOfComponent = sizeof(int);
            break;
        case GL_FLOAT:
            // Can't have floating point indices.
            ASSERT(_bufferType != GL_ELEMENT_ARRAY_BUFFER);
            sizeOfComponent = sizeof(float);
            break;
        case GL_DOUBLE:
            // Can't have floating point indices.
            ASSERT(_bufferType != GL_ELEMENT_ARRAY_BUFFER);
            sizeOfComponent = sizeof(double);
            break;
        default:
            THROW(InvalidStateError, "The specified type is invalid.");
    }

    _byteCount = sizeOfComponent * _elementSize * _elementCount;

    if (_accessType) {
        glGenBuffers(1, &_handle);
        glBindBuffer(_bufferType, _handle);
        glBufferData(_bufferType, _byteCount, data, _accessType);
        glBindBuffer(_bufferType, 0);
    }
}

Buffer::~Buffer() {
    if (_handle) {
        glDeleteBuffers(1, &_handle);
    }
}

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
