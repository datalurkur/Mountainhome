/*
 *  GenericAttributeBuffer.cpp
 *  Render
 *
 *  Created by loch on 12/15/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include <Base/Assertion.h>
#include "GenericAttributeBuffer.h"

GenericAttributeBuffer::GenericAttributeBuffer(
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
    ),
    _activeChannel(-1)
{}

void GenericAttributeBuffer::enable(int channel) {
    ASSERT(_activeChannel == -1);
    _activeChannel = channel;

    glEnableVertexAttribArray(_activeChannel);

    if (_handle) {
        glBindBuffer(_bufferType, _handle);
    }

    glVertexAttribPointer(_activeChannel, _elementSize, _dataType, GL_FALSE, 0, _handle ? 0 : _data);
}

void GenericAttributeBuffer::disable() {
    ASSERT(_activeChannel != -1);

    glDisableVertexAttribArray(_activeChannel);

    glVertexAttribPointer(_activeChannel, 4, GL_FLOAT, GL_FALSE, 0, 0);

    _activeChannel = -1;
}
