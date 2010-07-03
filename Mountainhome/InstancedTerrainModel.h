/*
 *  InstancedTerrainModel.h
 *  Mountainhome
 *
 *  Created by loch on 7/3/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _INSTANCEDTERRAINMODEL_H_
#define _INSTANCEDTERRAINMODEL_H_
#include "MHTerrainModel.h"

class MHInstancedTerrainModel : public MHTerrainModel {
public:
    MHInstancedTerrainModel(unsigned int *indices, int indexCount, Vector3 *verts, Vector3 *norms, Vector2 *texCoords, int vertexCount);
    virtual ~MHInstancedTerrainModel();

    void render(RenderContext *context);

protected:
    unsigned int *_indices;
    int _indexCount;

};

#endif
