/*
 *  MHModel.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

///\todo XXXBMW: Needs cleanup!!!!

#ifndef _MHMODEL_H_
#define _MHMODEL_H_
#include <Render/Model.h>

class RenderContext;
class MHModel : public Model {
public:
    MHModel(Vector3 *verts, Vector3 *norms, Vector2 *texCoords, int vertexCount);
    virtual ~MHModel();

    void render(RenderContext *context);

    virtual void clear();

protected:
    MHModel();
    void findBounds();

protected:
    Vector2 *_texCoords;
    Vector3 *_verts;
    Vector3 *_norms;
    int _count;
};

#endif
