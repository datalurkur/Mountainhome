/*
 *  NormalBuffer.cpp
 *  Mountainhome
 *
 *  Created by loch on 2/25/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#include "NormalBuffer.h"

NormalBuffer::NormalBuffer(
    GLenum accessType,
    GLenum dataType,
    unsigned int elementCount,
    void *data
):
    Buffer(
        GL_ARRAY_BUFFER,
        accessType,
        dataType,
        3,
        elementCount,
        data
    )
{}

void NormalBuffer::enable() {
    if (_handle) {
        glEnableClientState(GL_NORMAL_ARRAY);
        glBindBuffer(_bufferType, _handle);
    }

    glNormalPointer(_dataType, 0, _handle ? 0 : _data);

    if (_handle) {
        glBindBuffer(_bufferType, 0);
    }
}

void NormalBuffer::disable() {
    if (_handle) {
        glDisableClientState(GL_NORMAL_ARRAY);
    }

    glNormalPointer(GL_FLOAT, 0, 0);
}
