/*
 *  MHSceneManager.cpp
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include <Render/TextureManager.h>
#include <Render/MaterialManager.h>
#include <Render/Entity.h>
#include <Render/Model.h>
#include <Render/Node.h>

#include "MHSceneManager.h"
#include "MHWorld.h"

MHSceneManager::MHSceneManager(MHWorld *world, MaterialManager *manager):
_materialManager(manager), _world(world), _octreeMaxDepth(8),
_tileWidth(1.0), _tileHeight(1.0), _tileDepth(0.8) {}

MHSceneManager::~MHSceneManager() {}

WorldEntity::WorldEntity(Vector3 *verts, Vector3 *norms, Vector2 *texCoords, int vertexCount):
_texCoords(texCoords), _verts(verts), _norms(norms), _count(vertexCount) {
    for (int i = 0; i < _count; i++) {
        if (i == 0) { _boundingBox.setCenter(verts[i]); }
        else        { _boundingBox.encompass(verts[i]); }
    }
}

WorldEntity::~WorldEntity() {
    delete []_verts;     _verts = NULL;
    delete []_norms;     _norms = NULL;
    delete []_texCoords; _texCoords = NULL;
}

void WorldEntity::render(RenderContext *context) {
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


IndexedWorldEntity::IndexedWorldEntity(unsigned int *indices, int indexCount, Vector3 *verts, Vector3 *norms, Vector2 *texCoords, int vertexCount):
WorldEntity(verts, norms, texCoords, vertexCount), _indices(indices), _indexCount(indexCount) {}

IndexedWorldEntity::~IndexedWorldEntity() {
    delete []_indices; _indices = NULL;
}

void IndexedWorldEntity::render(RenderContext *context) {
    context->addToPrimitiveCount(_indexCount / 3 * 2);
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

    glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, _indices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    if (_norms) {
        glDisable(GL_LIGHTING);

        glUseProgramObjectARB(0);

        glActiveTextureARB(GL_TEXTURE0_ARB);
        glBindTexture(GL_TEXTURE_2D, NULL);
        glDisable(GL_TEXTURE_2D);

        glActiveTextureARB(GL_TEXTURE1_ARB);
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

        glEnable(GL_LIGHTING);
    }
}
