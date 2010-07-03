/*
 *  MHTerrainModel.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHTERRAINMODEL_H_
#define _MHTERRAINMODEL_H_
#include <Render/Model.h>

class RenderContext;
class MHTerrainModel : public Model {
public:
    MHTerrainModel(Vector3 *verts, Vector3 *norms, Vector2 *texCoords, int vertexCount);
    virtual ~MHTerrainModel();

    void render(RenderContext *context);

protected:
    Vector2 *_texCoords;
    Vector3 *_verts;
    Vector3 *_norms;
    int _count;
};

#endif
