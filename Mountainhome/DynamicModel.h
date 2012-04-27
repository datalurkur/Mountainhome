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
class DynamicModelVertex;
class DynamicModelFace;
class RenderOperation;
class VertexArray;
class IndexBuffer;

class DynamicModel {
public:
    enum WorldNormal {
        XY_POS,
        XY_NEG,
        YZ_POS,
        YZ_NEG,
        XZ_POS,
        XZ_NEG,
        WORLD_NORMAL_SIZE
    };

public:
    DynamicModel(int width, int height, int xOffset = 0, int yOffset = 0, int zOffset = 0);
    ~DynamicModel();

    int getVertexCount();
    int getIndexCount();

    RenderOperation * generateRenderOp(bool doPolyReduction);

    void addFace(
        Real x1, Real y1, Real z1,
        Real x2, Real y2, Real z2,
        Real x3, Real y3, Real z3,
        WorldNormal normal);

    void clearModel();

private:
    DynamicModelVertex *addVertex(Real x, Real y, Real z, WorldNormal normal);

    void cullModelPrims();

private:
    std::vector<Vector3> _vertsArray;

    TranslationMatrix *_matrix;
    DynamicModelVertex *_baseVertex;
    DynamicModelFace *_baseFace;

    int _vertexCount;
    int _indexCount;

    int _xOffset;
    int _yOffset;
    int _zOffset;

    int _width;
    int _height;

    RenderOperation *_renderOp;
    VertexArray *_vertexArray;
    IndexBuffer *_indexBuffer;

};

#endif
