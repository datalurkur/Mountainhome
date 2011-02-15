/*
 *  AttributeBuffer.cpp
 *  Render
 *
 *  Created by loch on 12/15/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include <Base/Assertion.h>
#include "AttributeBuffer.h"

AttributeBuffer::AttributeBuffer(
    GLenum accessType,
    GLenum dataType,
    unsigned int elementSize,
    unsigned int elementCount,
    void *data
):
    Buffer(
        GL_ARRAY_BUFFER,
        accessType,
        dataType,
        elementCount,
        data
    ),
    _elementSize(elementSize),
    _activeChannel(-1)
{
    int sizeOfComponent = 0;
    
    switch(_dataType) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
            sizeOfComponent = sizeof(char);
            break;
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
            sizeOfComponent = sizeof(short);
            break;
        case GL_INT:
        case GL_UNSIGNED_INT:
            sizeOfComponent = sizeof(int);
            break;
        case GL_FLOAT:
            sizeOfComponent = sizeof(float);
            break;
        case GL_DOUBLE:
            sizeOfComponent = sizeof(double);
            break;
        default:
            THROW(InvalidStateError, "The specified type is invalid.");
    }

    _byteCount = sizeOfComponent * _elementSize * _elementCount;

    if (_accessType) {
        glGenBuffers(1, &_handle);
        glBindBuffer(GL_ARRAY_BUFFER, _handle);
        glBufferData(GL_ARRAY_BUFFER, _byteCount, data, _accessType);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

AttributeBuffer::~AttributeBuffer() {
    if (_handle) {
        glDeleteBuffers(1, &_handle);
    }
}

void AttributeBuffer::enable(int channel) {
    if (_activeChannel) {
        disable();
    }

    _activeChannel = channel;

    if (_handle) {
        glBindBuffer(GL_ARRAY_BUFFER, _handle);
    }

    glVertexAttribPointer(_activeChannel, _elementSize, _dataType, GL_FALSE, 0, _handle ? 0 : _data);
    glEnableVertexAttribArray(_activeChannel);
}

void AttributeBuffer::disable() {
    ASSERT(_activeChannel != -1);
    glDisableVertexAttribArray(_activeChannel);
    _activeChannel = -1;
}
