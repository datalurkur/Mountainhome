/*
 *  ShaderParameter.cpp
 *  Mountainhome
 *
 *  Created by loch on 12/20/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "ShaderParameter.h"

ShaderParameter::ShaderParameter(): _data(NULL), _type(FloatParam), _size(0), _count(0), _free(false) {}

ShaderParameter::~ShaderParameter() {
    if (_free && _data) {
        free(_data);
        _data = NULL;
    }
}

void ShaderParameter::setData(void *data, ShaderParameterType type, int size, int count, bool needsFree) {
    if (_data && _free) {
        free(_data);
    }

    _data = data;
    _type = type;
    _size = size;
    _count = count;
    _free = needsFree;

    if (_type != FloatParam && _type != IntParam && _size > 1) {
        THROW(InternalError, "Only int and float types can have sizes.");
    }
}

ShaderParameterType ShaderParameter::getType()  { return _type;  }
int                 ShaderParameter::getCount() { return _count; }
int                 ShaderParameter::getSize()  { return _size;  }

template <>
void ShaderParameter::setData(float *data, int count, bool free) {
    setData(data, FloatParam, 1, count, free);
}

template <>
void ShaderParameter::setData(Vector2 *data, int count, bool free) {
    setData(data, FloatParam, 2, count, free);
}

template <>
void ShaderParameter::setData(Vector3 *data, int count, bool free) {
    setData(data, FloatParam, 3, count, free);
}

template <>
void ShaderParameter::setData(Vector4 *data, int count, bool free) {
    setData(data, FloatParam, 4, count, free);
}

template <>
void ShaderParameter::setData(int *data, int count, bool free) {
    setData(data, IntParam, 1, count, free);
}

template <>
void ShaderParameter::setData(IVector2 *data, int count, bool free) {
    setData(data, IntParam, 2, count, free);
}

template <>
void ShaderParameter::setData(IVector3 *data, int count, bool free) {
    setData(data, IntParam, 3, count, free);
}

template <>
void ShaderParameter::setData(IVector4 *data, int count, bool free) {
    setData(data, IntParam, 4, count, free);
}

template <>
void ShaderParameter::setData(Matrix *data, int count, bool free) {
    setData(data, Matrix4Param, 1, count, free);
}

template <>
void ShaderParameter::setData(Texture *data, int count, bool free) {
    setData(data, TextureParam, 1, count, free);
}
