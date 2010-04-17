/*
 *  MHWorldModel.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include <Render/RenderContext.h>
#include "MHWorldModel.h"

MHWorldModel::MHWorldModel(Vector3 *verts, Vector3 *norms, Vector2 *texCoords, int vertexCount):
_texCoords(texCoords), _verts(verts), _norms(norms), _count(vertexCount) {
    for (int i = 0; i < _count; i++) {
        if (i == 0) { _boundingBox.setCenter(verts[i]); }
        else        { _boundingBox.encompass(verts[i]); }
    }
}

MHWorldModel::~MHWorldModel() {
    delete []_verts;     _verts = NULL;
    delete []_norms;     _norms = NULL;
    delete []_texCoords; _texCoords = NULL;
}

void MHWorldModel::render(RenderContext *context) {
    context->addToPrimitiveCount(_count / 3 * 2);
    context->addToVertexCount(_count * 2);
    context->addToModelCount(1);

    if (_verts) {
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, _verts);
    }

    if (_norms) {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, _norms);
    }

    if (_texCoords) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, _texCoords);
    }

    context->setFilled();
    glDrawArrays(GL_TRIANGLES, 0, _count);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
