/*
 *  MHSceneManager.cpp
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include <Render/MaterialManager.h>
#include <Render/Entity.h>
#include <Render/Model.h>
#include <Render/Node.h>

#include "MHSceneManager.h"
#include "MHWorld.h"

class TileChunk : public Model {
public:
    TileChunk(Vector3 *verts, Vector3 *norms, int count): _verts(verts), _norms(norms), _count(count) {
        for (int i = 0; i < count; i++) {
            if (i == 0) {
                _boundingBox.setCenter(verts[i]);
            } else {
                _boundingBox.encompass(verts[i]);
            }
        }
    }

    virtual ~TileChunk() {
        delete []_verts; _verts = NULL;
        delete []_norms; _norms = NULL;
    }

    void render(RenderContext *context) {
        context->addToPrimitiveCount(_count);
        context->addToVertexCount(_count * 3);
        context->addToModelCount(1);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, _verts);
        glNormalPointer(GL_FLOAT, 0, _norms);
        glDrawArrays(GL_TRIANGLES, 0, _count);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
    }

private:
    Vector3 *_verts;
    Vector3 *_norms;
    int _count;
};

MHSceneManager::MHSceneManager(MHWorld *world): _world(world), _octreeMaxDepth(8),
_tileChunkWidth(8), _tileChunkHeight(8), _tileChunkDepth(8),
_tileWidth(1.0), _tileHeight(1.0), _tileDepth(1.0) {

}

MHSceneManager::~MHSceneManager() {

}

void MHSceneManager::populate() {
    for (int x = 0; x < _world->getWidth(); x+=_tileChunkWidth) {
        for (int y = 0; y < _world->getHeight(); y+=_tileChunkHeight) {
            for (int z = 0; z < _world->getDepth(); z+=_tileChunkDepth) {
                createChunk(x, y, z);
            }
        }    
    }
}

void MHSceneManager::createChunk(int x, int y, int z) {
    // Get the chunk name
    char chunkName[32];
    snprintf(chunkName, 31, "[%i, %i, %i]", x, y, z);

    // Build the chunk geometry
    std::vector<Vector3> vertArray;
    for (int xIndexOffset = 0; xIndexOffset < _tileChunkWidth; xIndexOffset++) {
        for (int yIndexOffset = 0; yIndexOffset < _tileChunkHeight; yIndexOffset++) {
            for (int zIndexOffset = 0; zIndexOffset < _tileChunkDepth; zIndexOffset++) {
                // Don't consider areas that are out of bounds.
                if (x + xIndexOffset >= _world->getWidth()  ||
                    y + yIndexOffset >= _world->getHeight() ||
                    z + zIndexOffset >= _world->getDepth()) {
                    continue;
                }

                // Get the tile and make sure it is a valid, filled tile.
                MHWorld::Tile *t = _world->getTile(x + xIndexOffset, y + yIndexOffset, z + zIndexOffset);
                if (!t->type) {
                    continue;
                }

                // Move this tile over based on it's offset within the chunk.
                Real xOffset = xIndexOffset * _tileWidth;
                Real yOffset = yIndexOffset * _tileHeight;
                Real zOffset = zIndexOffset * _tileDepth;

                // And build some actual geometry.
                { // Top
                    // Bottom left triangle.
                    vertArray.push_back(Vector3(xOffset,              yOffset,               zOffset + _tileDepth));
                    vertArray.push_back(Vector3(xOffset + _tileWidth, yOffset,               zOffset + _tileDepth));
                    vertArray.push_back(Vector3(xOffset,              yOffset + _tileHeight, zOffset + _tileDepth));
                    // Top right triangle.
                    vertArray.push_back(Vector3(xOffset,              yOffset + _tileHeight, zOffset + _tileDepth));
                    vertArray.push_back(Vector3(xOffset + _tileWidth, yOffset,               zOffset + _tileDepth));
                    vertArray.push_back(Vector3(xOffset + _tileWidth, yOffset + _tileHeight, zOffset + _tileDepth));
                }

                { // West Wall
                    // Bottom left triangle.
                    vertArray.push_back(Vector3(xOffset,              yOffset + _tileHeight, zOffset         ));
                    vertArray.push_back(Vector3(xOffset,              yOffset,               zOffset         ));
                    vertArray.push_back(Vector3(xOffset,              yOffset + _tileHeight, zOffset + _tileDepth));
                    // Top right triangle.
                    vertArray.push_back(Vector3(xOffset,              yOffset + _tileHeight, zOffset + _tileDepth));
                    vertArray.push_back(Vector3(xOffset,              yOffset,               zOffset         ));
                    vertArray.push_back(Vector3(xOffset,              yOffset,               zOffset + _tileDepth));
                }

                { // East Wall
                    // Bottom left triangle.
                    vertArray.push_back(Vector3(xOffset + _tileWidth, yOffset,               zOffset         ));
                    vertArray.push_back(Vector3(xOffset + _tileWidth, yOffset + _tileHeight, zOffset         ));
                    vertArray.push_back(Vector3(xOffset + _tileWidth, yOffset,               zOffset + _tileDepth));
                    // Top right triangle.
                    vertArray.push_back(Vector3(xOffset + _tileWidth, yOffset,               zOffset + _tileDepth));
                    vertArray.push_back(Vector3(xOffset + _tileWidth, yOffset + _tileHeight, zOffset         ));
                    vertArray.push_back(Vector3(xOffset + _tileWidth, yOffset + _tileHeight, zOffset + _tileDepth));
                }

                { // North Wall
                    // Bottom left triangle.
                    vertArray.push_back(Vector3(xOffset + _tileWidth, yOffset + _tileHeight, zOffset));
                    vertArray.push_back(Vector3(xOffset,              yOffset + _tileHeight, zOffset));
                    vertArray.push_back(Vector3(xOffset + _tileWidth, yOffset + _tileHeight, zOffset + _tileDepth));
                    // Top right triangle.
                    vertArray.push_back(Vector3(xOffset + _tileWidth, yOffset + _tileHeight, zOffset + _tileDepth));
                    vertArray.push_back(Vector3(xOffset,              yOffset + _tileHeight, zOffset         ));
                    vertArray.push_back(Vector3(xOffset,              yOffset + _tileHeight, zOffset + _tileDepth));
                }

                { // South Wall
                    // Bottom left triangle.
                    vertArray.push_back(Vector3(xOffset,              yOffset,               zOffset         ));
                    vertArray.push_back(Vector3(xOffset + _tileWidth, yOffset,               zOffset         ));
                    vertArray.push_back(Vector3(xOffset,              yOffset,               zOffset + _tileDepth));
                    // Top right triangle.
                    vertArray.push_back(Vector3(xOffset,              yOffset,               zOffset + _tileDepth));
                    vertArray.push_back(Vector3(xOffset + _tileWidth, yOffset,               zOffset         ));
                    vertArray.push_back(Vector3(xOffset + _tileWidth, yOffset,               zOffset + _tileDepth));
                }
            }
        }    
    }

    Vector3 *verts = new Vector3[vertArray.size()];
    Vector3 *norms = new Vector3[vertArray.size()];
    for (int i = 0; i < vertArray.size(); i+=3) {
		Vector3 one = vertArray[i+1] - vertArray[i+0];
		Vector3 two = vertArray[i+2] - vertArray[i+1];
		Vector3 normal = one.crossProduct(two);
        normal.normalize();

        for (int j = 0; j < 3; j++) {
            verts[i+j] = vertArray[i+j];
            norms[i+j] = normal;
        }
    }

    // Create the chunk and place it in the world.
    TileChunk *chunk = new TileChunk(verts, norms, vertArray.size());
    Entity *entity = createEntity(chunk, chunkName);
    entity->setMaterial(MaterialManager::Get()->loadResource("white"));
    entity->setPosition(x * _tileWidth, y * _tileHeight, z * _tileDepth);
    getRootNode()->attach(entity);
}
