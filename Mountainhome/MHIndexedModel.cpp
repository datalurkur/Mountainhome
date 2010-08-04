/*
 *  MHMHIndexedModel.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include <Render/RenderContext.h>

#include "MHIndexedModel.h"

MHIndexedModel::MHIndexedModel(): _indices(NULL), _indexCount(0) {}

MHIndexedModel::MHIndexedModel(unsigned int *indices, int indexCount, Vector3 *verts, Vector3 *norms, Vector2 *texCoords, int vertexCount):
MHModel(verts, norms, texCoords, vertexCount), _indices(indices), _indexCount(indexCount) {
    /*Info("WORLD MODEL:");
    LogStream::IncrementIndent();
    for (int i = 0; i < vertexCount; i++) {
        Info("[" << i << "] "
             "(" << verts[i].x << ", " << verts[i].y << ", " << verts[i].z << ") "
             "(" << norms[i].x << ", " << norms[i].y << ", " << norms[i].z << ") "
             "(" << texCoords[i].x << ", " << texCoords[i].y << ") ");
    }
    LogStream::DecrementIndent();*/
}

MHIndexedModel::~MHIndexedModel() {
    delete []_indices; _indices = NULL;
}

void MHIndexedModel::render(RenderContext *context) {
    context->addToPrimitiveCount(_indexCount / 3 * 2);
    context->addToVertexCount(_count * 2);
    context->addToModelCount(1);

//    context->setWireFrame();

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

    glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, _indices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

//    context->setFilled();

    if (0 && _norms) {
        glDisable(GL_LIGHTING);

        glUseProgram(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, NULL);
        glDisable(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, NULL);
        glDisable(GL_TEXTURE_2D);

        glBegin(GL_LINES);
        for (int i = 0; i < _count; i++) {
            float color = pow(_norms[i].z, 5.0);
            glColor4f(1.0, color, color, 1.0);
            glVertex3fv(_verts[i].array);
            glVertex3fv((_verts[i] + _norms[i]).array);
        }
        glEnd();
    }
}
