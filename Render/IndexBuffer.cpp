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
        1,
        count,
        data
    )
{}

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
