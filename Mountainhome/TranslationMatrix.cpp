/*
 *  TranslationMatrix.cpp
 *  Mountainhome
 *
 *  Created by loch on 8/4/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "TranslationMatrix.h"
#include <Base/Assertion.h>

TranslationMatrix::TranslationMatrix(int width, int height): _width(width), _height(height) {
    _matrix = new ZMap[width * height * 6];
}

TranslationMatrix::~TranslationMatrix() {
    delete[] _matrix;
}

void TranslationMatrix::setVertex(int x, int y, int z, int normal, DynamicModelVertex *vertex) {
    if (x < 0) { x = _width  - 1 ; }
    if (y < 0) { y = _height - 1 ; }
    ASSERT_LE(x, _width  - 1);
    ASSERT_LE(y, _height - 1);

    _matrix[(normal * _width * _height) + (y * _width) + x][z] = vertex;
}

DynamicModelVertex *TranslationMatrix::getVertex(int x, int y, int z, int normal) {
    if (x < 0) { x = _width  - 1 ; }
    if (y < 0) { y = _height - 1 ; }
    ASSERT_LE(x, _width  - 1);
    ASSERT_LE(y, _height - 1);

    ZMap *zmap = _matrix + (normal * _width * _height) + (y * _width) + x;

    ZMap::iterator itr = zmap->find(z);
    return itr == zmap->end() ? NULL : itr->second;
}
