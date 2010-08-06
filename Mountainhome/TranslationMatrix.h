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

class DynamicModelIndex;
class TranslationMatrix {
public:
    TranslationMatrix(int width, int height);
    virtual ~TranslationMatrix();

    void setIndex(int x, int y, int z, DynamicModelIndex *index);
    DynamicModelIndex* getIndex(int x, int y, int z);

private:
    typedef std::map<int, DynamicModelIndex*> ZMap;

private:
    ZMap *_matrix;
    int _width;
    int _height;

};

#endif
