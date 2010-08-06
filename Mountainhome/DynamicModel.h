/*
 *  DynamicModel.h
 *  Mountainhome
 *
 *  Created by loch on 8/4/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _DYNAMICMODEL_H_
#define _DYNAMICMODEL_H_
#include <Base/Vector.h>

class TranslationMatrix;
class DynamicModelIndex;
class DynamicModelFace;
class DynamicModel {
public:
    enum PlaneFlags {
        XY = 1 << 0,
        YZ = 1 << 1,
        ZX = 1 << 2
    };

public:
    DynamicModel(int width, int height, int xOffset = 0, int yOffset = 0, int zOffset = 0);
    ~DynamicModel();

    int getVertexCount();
    int getIndexCount();

    Vector3 *buildStaticVertexArray();
    Vector2 *buildStaticTexCoordArray();
    unsigned int *buildStaticIndexArray();

    void doPolyReduction();

    void addFace(
        Real x1, Real y1, Real z1,
        Real x2, Real y2, Real z2,
        Real x3, Real y3, Real z3,
        int plane);

private:
    DynamicModelIndex *addVertex(Real x, Real y, Real z);

private:
    std::vector<Vector3> _vertsArray;
    std::vector<Vector2> _texCoordsArray;

    TranslationMatrix *_matrix;
    DynamicModelIndex *_baseIndex;
    DynamicModelFace *_baseFace;

    int _xOffset;
    int _yOffset;
    int _zOffset;

};

#endif