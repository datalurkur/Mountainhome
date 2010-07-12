/*
 *  MHReducedModel.h
 *  Mountainhome
 *
 *  Created by loch on 4/17/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHREDUCEDMODEL_H_
#define _MHREDUCEDMODEL_H_
#include "MHModel.h"

class LODIndexArray;
class MHReducedModel : public MHModel {
public:
    MHReducedModel(unsigned int *indices, int indexCount, Vector3 *verts, Vector3 *norms, Vector2 *texCoords, int vertexCount);
    virtual ~MHReducedModel();

    void render(RenderContext *context);

protected:
    MHReducedModel();

protected:
    LODIndexArray *_indices;

};

#endif
