/*
 *  MHMHIndexedModel.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHINDEXEDMODEL_H_
#define _MHINDEXEDMODEL_H_
#include "MHModel.h"

class MHIndexedModel : public MHModel {
public:
    MHIndexedModel(unsigned int *indices, int indexCount, Vector3 *verts, Vector3 *norms, Vector2 *texCoords, int vertexCount);
    virtual ~MHIndexedModel();

    void render(RenderContext *context);

protected:
    unsigned int *_indices;
    int _indexCount;

};

#endif
