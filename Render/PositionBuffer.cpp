/*
 *  PositionBuffer.cpp
 *  Mountainhome
 *
 *  Created by loch on 2/25/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#include "PositionBuffer.h"

PositionBuffer::PositionBuffer(
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
        elementSize,
        elementCount,
        data
    )
{}

void PositionBuffer::enable() {
    if (_handle) {
        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(_bufferType, _handle);
    }

    glVertexPointer(_elementSize, _dataType, 0, _handle ? 0 : _data);

    if (_handle) {
        glBindBuffer(_bufferType, 0);
    }
}

void PositionBuffer::disable() {
    if (_handle) {
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    glVertexPointer(4, GL_FLOAT, 0, 0);
}
