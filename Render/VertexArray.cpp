/*
 *  VertexArray.cpp
 *  Mountainhome
 *
 *  Created by loch on 12/17/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "VertexArray.h"

VertexArray::VertexArray() {}
VertexArray::~VertexArray() {}

unsigned int VertexArray::getElementCount() const {
    return getAttributeCount() == 0 ? 0 : _buffers[0]->getElementCount();
}

unsigned int VertexArray::getAttributeCount() const {
    return _buffers.size();
}

unsigned int VertexArray::addAttributeBuffer(const std::string &name, AttributeBuffer *buffer) {
    _buffers.push_back(buffer);
    _names.push_back(name);

    return getAttributeCount() - 1;
}

AttributeBuffer * VertexArray::getAttributeBuffer(unsigned int index) const {
    return _buffers[index];
}

AttributeBuffer * VertexArray::getAttributeBuffer(const std::string &name) const {
    for (int i = 0; i < getAttributeCount(); i++) {
        if (_names[i] == name) {
            return getAttributeBuffer(i);
        }
    }

    THROW(InternalError, "The named AttributeBuffer does not exist.");
    return NULL;
}

const VertexArrayLayout & VertexArray::getVertexArrayLayout() const {
    return _names;
}

void VertexArray::enable() {
    for (int i = 0; i < getAttributeCount(); i++) {
        _buffers[i]->enable(i);
    }
}

void VertexArray::disable() {
    for (int i = 0; i < getAttributeCount(); i++) {
        _buffers[i]->disable();
    }
}
