/*
 *  MHModel.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include <Render/RenderContext.h>
#include "MHModel.h"

MHModel::MHModel():_texCoords(NULL), _verts(NULL), _norms(NULL), _count(0) {}

MHModel::MHModel(Vector3 *verts, Vector3 *norms, Vector2 *texCoords, int vertexCount):
_texCoords(texCoords), _verts(verts), _norms(norms), _count(vertexCount) {
    findBounds();
}

MHModel::~MHModel() {
    clear();
}

void MHModel::clear() {
    if (_verts)     { delete []_verts;     _verts     = NULL; }
    if (_norms)     { delete []_norms;     _norms     = NULL; }
    if (_texCoords) { delete []_texCoords; _texCoords = NULL; }
    _count = 0;
}

void MHModel::findBounds() {
    _boundingBox.setRadius(Vector3(0, 0, 0));
    for (int i = 0; i < _count; i++) {
        if (i == 0) { _boundingBox.setCenter(_verts[i]); }
        else        { _boundingBox.encompass(_verts[i]); }
    }
}

void MHModel::render(RenderContext *context) {
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

    // context->setFilled();
    glDrawArrays(GL_TRIANGLES, 0, _count);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
