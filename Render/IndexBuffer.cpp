/*
 *  IndexBuffer.cpp
 *  Render
 *
 *  Created by loch on 12/15/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(
    GLenum accessType,
    GLenum dataType,
    unsigned int count,
    void *data
):
    Buffer(
        GL_ELEMENT_ARRAY_BUFFER,
        accessType,
        dataType,
        count,
        data
    )
{
    int sizeOfElement = 0;
    
    switch(_dataType) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
            sizeOfElement = sizeof(char);
            break;
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
            sizeOfElement = sizeof(short);
            break;
        case GL_INT:
        case GL_UNSIGNED_INT:
            sizeOfElement = sizeof(int);
            break;
        default:
            THROW(InvalidStateError, "The specified type is invalid.");
    }

    _byteCount = sizeOfElement * _elementCount;

    if (_accessType) {
        glGenBuffers(1, &_handle);
        glBindBuffer(_bufferType, _handle);
        glBufferData(_bufferType, _byteCount, data, _accessType);
        glBindBuffer(_bufferType, 0);
    }
}

IndexBuffer::~IndexBuffer() {
    if (_handle) {
        glDeleteBuffers(1, &_handle);
    }
}

void IndexBuffer::render(PrimitiveType type) {
    if (_handle) { glBindBuffer(_bufferType, _handle); }

    glDrawElements(
        TranslatePrimitiveType(type),
        getElementCount(),
        getDataType(),
        _handle ? 0 : _data
    );

    if (_handle) { glBindBuffer(_bufferType, 0); }
}
