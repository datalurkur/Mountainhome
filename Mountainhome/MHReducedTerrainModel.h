/*
 *  MHReducedTerrainModel.h
 *  Mountainhome
 *
 *  Created by loch on 4/17/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _REDUCEDTERRAINMODEL_H_
#define _REDUCEDTERRAINMODEL_H_
#include "MHTerrainModel.h"

class LODIndexArray;
class MHReducedTerrainModel : public MHTerrainModel {
public:
    MHReducedTerrainModel(unsigned int *indices, int indexCount, Vector3 *verts, Vector3 *norms, Vector2 *texCoords, int vertexCount);
    virtual ~MHReducedTerrainModel();

    void render(RenderContext *context);

protected:
    LODIndexArray *_indices;

};

#endif
