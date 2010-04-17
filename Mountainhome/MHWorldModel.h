/*
 *  MHWorldModel.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHWORLDMODEL_H_
#define _MHWORLDMODEL_H_
#include <Render/Model.h>

class RenderContext;
class MHWorldModel : public Model {
public:
    MHWorldModel(Vector3 *verts, Vector3 *norms, Vector2 *texCoords, int vertexCount);
    virtual ~MHWorldModel();

    void render(RenderContext *context);

protected:
    Vector2 *_texCoords;
    Vector3 *_verts;
    Vector3 *_norms;
    int _count;
};

#endif
