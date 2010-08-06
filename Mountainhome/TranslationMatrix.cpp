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
    _matrix = new ZMap[width * height];
}

TranslationMatrix::~TranslationMatrix() {
    delete[] _matrix;
}

void TranslationMatrix::setIndex(int x, int y, int z, DynamicModelIndex *index) {
    if (x < 0) { x = _width  - 1 ; }
    if (y < 0) { y = _height - 1 ; }
    ASSERT_LE(x, _width  - 1);
    ASSERT_LE(y, _height - 1);

    _matrix[y * _width + x][z] = index;
}

DynamicModelIndex *TranslationMatrix::getIndex(int x, int y, int z) {
    if (x < 0) { x = _width  - 1 ; }
    if (y < 0) { y = _height - 1 ; }
    ASSERT_LE(x, _width  - 1);
    ASSERT_LE(y, _height - 1);

    ZMap::iterator itr = _matrix[y * _width + x].find(z);
    return itr == _matrix[y * _width + x].end() ? NULL : itr->second;
}
