/*
 *  TranslationMatrix.h
 *  Mountainhome
 *
 *  Created by loch on 8/4/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _TRANSLATIONMATRIX_H_
#define _TRANSLATIONMATRIX_H_
#include <map>

class DynamicModelVertex;
class TranslationMatrix {
public:
    TranslationMatrix(int width, int height);
    virtual ~TranslationMatrix();

    void setVertex(int x, int y, int z, int normal, DynamicModelVertex *vertex);

    DynamicModelVertex* getVertex(int x, int y, int z, int normal);

private:
    typedef std::map<int, DynamicModelVertex*> ZMap;

private:
    ZMap *_matrix;
    int _width;
    int _height;

};

#endif
