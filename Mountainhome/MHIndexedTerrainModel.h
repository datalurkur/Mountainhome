/*
 *  MHMHIndexedTerrainModel.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _INDEXEDTERRAINMODEL_H_
#define _INDEXEDTERRAINMODEL_H_
#include "MHTerrainModel.h"

class MHIndexedTerrainModel : public MHTerrainModel {
public:
    MHIndexedTerrainModel(unsigned int *indices, int indexCount, Vector3 *verts, Vector3 *norms, Vector2 *texCoords, int vertexCount);
    virtual ~MHIndexedTerrainModel();

    void render(RenderContext *context);

protected:
    unsigned int *_indices;
    int _indexCount;

};

#endif
