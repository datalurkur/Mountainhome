/*
 *  MHReducedWorldModel.h
 *  Mountainhome
 *
 *  Created by loch on 4/17/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _REDUCEDWORLDMODEL_H_
#define _REDUCEDWORLDMODEL_H_
#include "MHWorldModel.h"

class LODIndexArray;
class MHReducedWorldModel : public MHWorldModel {
public:
    MHReducedWorldModel(unsigned int *indices, int indexCount, Vector3 *verts, Vector3 *norms, Vector2 *texCoords, int vertexCount);
    virtual ~MHReducedWorldModel();

    void render(RenderContext *context);

protected:
    LODIndexArray *_indices;

};

#endif
