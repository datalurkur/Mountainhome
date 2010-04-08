/*
 *  MHMHIndexedWorldModel.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _INDEXEDWORLDMODEL_H_
#define _INDEXEDWORLDMODEL_H_
#include "MHWorldModel.h"

class MHIndexedWorldModel : public MHWorldModel {
public:
    MHIndexedWorldModel(unsigned int *indices, int indexCount, Vector3 *verts, Vector3 *norms, Vector2 *texCoords, int vertexCount);
    virtual ~MHIndexedWorldModel();

    void render(RenderContext *context);

protected:
    unsigned int *_indices;
    int _indexCount;

};

#endif
