/*
 *  TestSceneManager1.cpp
 *  Mountainhome
 *
 *  Created by loch on 3/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include <Render/MaterialManager.h>
#include <Render/Entity.h>
#include <Render/Model.h>
#include <Render/Node.h>

#include "TestSceneManager1.h"
#include "MHWorld.h"

TestSceneManager1::TestSceneManager1(MHWorld *world, MaterialManager *manager): MHSceneManager(world, manager),
_voxelChunkWidth(128), _voxelChunkHeight(128), _voxelChunkDepth(128) {}

TestSceneManager1::~TestSceneManager1() {}

void TestSceneManager1::populate() {
    for (int x = 0; x < _world->getWidth(); x+=_voxelChunkWidth) {
        for (int y = 0; y < _world->getHeight(); y+=_voxelChunkHeight) {
            for (int z = 0; z < _world->getDepth(); z+=_voxelChunkDepth) {
                createChunk(x, y, z);
            }
        }    
    }
}

Real TestSceneManager1::determineZDelta(MHWorld::Voxel *base, int x, int y) {
    #define CONSIDER_TILE(deltaX, deltaY) \
    do { \
        if (t = _world->getTopVoxel(base->x + x - (deltaX), base->y + y - (deltaY))) { \
            current = std::max(current, t->z); \
        } \
    } while(0)

    MHWorld::Voxel *t;
    int current = 0;
    CONSIDER_TILE(0, 0);
    CONSIDER_TILE(1, 0);
    CONSIDER_TILE(0, 1);
    CONSIDER_TILE(1, 1);
    #undef CONSIDER_TILE

    return (current - base->z) * _voxelDepth;
}

void TestSceneManager1::createChunk(int x, int y, int z) {
    // Get the chunk name
    char chunkName[32];
    snprintf(chunkName, 31, "[%i, %i, %i]", x, y, z);

    // Build the chunk geometry
    std::vector<Vector3> vertArray;
    for (int xIndexOffset = 0; xIndexOffset < _voxelChunkWidth; xIndexOffset++) {
        for (int yIndexOffset = 0; yIndexOffset < _voxelChunkHeight; yIndexOffset++) {
            for (int zIndexOffset = 0; zIndexOffset < _voxelChunkDepth; zIndexOffset++) {
                // Don't consider areas that are out of bounds.
                if (x + xIndexOffset >= _world->getWidth()  ||
                    y + yIndexOffset >= _world->getHeight() ||
                    z + zIndexOffset >= _world->getDepth()) {
                    continue;
                }

                // Get the voxel and make sure it is a valid, filled voxel.
                MHWorld::Voxel *t = _world->getVoxel(x + xIndexOffset, y + yIndexOffset, z + zIndexOffset);
                if (!t->isFilled() || !t->isTopLevel()) {
                    continue;
                }

                // Move this voxel over based on its offset within the chunk.
                Real xOffset = xIndexOffset * _voxelWidth;
                Real yOffset = yIndexOffset * _voxelHeight;
                Real zOffset = zIndexOffset * _voxelDepth;

                // If we're at the top level, we need to look at the neighbors to
                // determine if a z offset is needed to properly connect voxels. If we're
                // not at the top level, just assume an offset of 0.
                bool topLevel = t->isTopLevel();
                Real swZPos   = topLevel ? determineZDelta(t, 0, 0) : 0;
                Real nwZPos   = topLevel ? determineZDelta(t, 0, 1) : 0;
                Real seZPos   = topLevel ? determineZDelta(t, 1, 0) : 0;
                Real neZPos   = topLevel ? determineZDelta(t, 1, 1) : 0;
                Real zPos     = zOffset + _voxelDepth;

                // If all four corners are raised to the neighbors, we've effectively
                // filled some unfilled voxels! For this reason, we need to do some special
                // geometry to sort of fake a hole.
                if (nwZPos > 0 && neZPos > 0 && swZPos > 0 && seZPos > 0) {
                    Real midX = xOffset + (_voxelWidth  / 2.0);
                    Real midY = yOffset + (_voxelHeight / 2.0);

                    // North triangle.
                    vertArray.push_back(Vector3(midX,                 midY,                  zPos  ));
                    vertArray.push_back(Vector3(xOffset + _voxelWidth, yOffset + _voxelHeight, zPos + neZPos));
                    vertArray.push_back(Vector3(xOffset,              yOffset + _voxelHeight, zPos + nwZPos));
                    // East triangle.
                    vertArray.push_back(Vector3(midX,                 midY,                  zPos  ));
                    vertArray.push_back(Vector3(xOffset + _voxelWidth, yOffset,               zPos + seZPos));
                    vertArray.push_back(Vector3(xOffset + _voxelWidth, yOffset + _voxelHeight, zPos + neZPos));
                    // South triangle
                    vertArray.push_back(Vector3(midX,                 midY,                  zPos  ));
                    vertArray.push_back(Vector3(xOffset,              yOffset,               zPos + swZPos));
                    vertArray.push_back(Vector3(xOffset + _voxelWidth, yOffset,               zPos + seZPos));
                    // West Triangle
                    vertArray.push_back(Vector3(midX,                 midY,                  zPos  ));
                    vertArray.push_back(Vector3(xOffset,              yOffset + _voxelHeight, zPos + nwZPos));
                    vertArray.push_back(Vector3(xOffset,              yOffset,               zPos + swZPos));
                } else {
                    // Bottom left triangle.
                    vertArray.push_back(Vector3(xOffset,              yOffset,               zPos + swZPos));
                    vertArray.push_back(Vector3(xOffset + _voxelWidth, yOffset,               zPos + seZPos));
                    vertArray.push_back(Vector3(xOffset,              yOffset + _voxelHeight, zPos + nwZPos));
                    // Top right triangle.
                    vertArray.push_back(Vector3(xOffset,              yOffset + _voxelHeight, zPos + nwZPos));
                    vertArray.push_back(Vector3(xOffset + _voxelWidth, yOffset,               zPos + seZPos));
                    vertArray.push_back(Vector3(xOffset + _voxelWidth, yOffset + _voxelHeight, zPos + neZPos));
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
    if (vertArray.size()) {
        Entity *entity = createEntity(new WorldEntity(verts, norms, NULL, vertArray.size()), chunkName);
        entity->setMaterial(_materialManager->getOrLoadResource("grass"));
        entity->setPosition(x * _voxelWidth, y * _voxelHeight, z * _voxelDepth);
        getRootNode()->attach(entity);
    }
}

//                    { // West Wall
//                        // Bottom left triangle.
//                        vertArray.push_back(Vector3(xOffset,              yOffset + _voxelHeight, zOffset));
//                        vertArray.push_back(Vector3(xOffset,              yOffset,               zOffset));
//                        vertArray.push_back(Vector3(xOffset,              yOffset + _voxelHeight, nwZPos ));
//                        // Top right triangle.
//                        vertArray.push_back(Vector3(xOffset,              yOffset + _voxelHeight, nwZPos ));
//                        vertArray.push_back(Vector3(xOffset,              yOffset,               zOffset));
//                        vertArray.push_back(Vector3(xOffset,              yOffset,               swZPos ));
//                    }
//
//                    { // East Wall
//                        // Bottom left triangle.
//                        vertArray.push_back(Vector3(xOffset + _voxelWidth, yOffset,               zOffset));
//                        vertArray.push_back(Vector3(xOffset + _voxelWidth, yOffset + _voxelHeight, zOffset));
//                        vertArray.push_back(Vector3(xOffset + _voxelWidth, yOffset,               seZPos ));
//                        // Top right triangle.
//                        vertArray.push_back(Vector3(xOffset + _voxelWidth, yOffset,               seZPos ));
//                        vertArray.push_back(Vector3(xOffset + _voxelWidth, yOffset + _voxelHeight, zOffset));
//                        vertArray.push_back(Vector3(xOffset + _voxelWidth, yOffset + _voxelHeight, neZPos ));
//                    }
//
//                    { // North Wall
//                        // Bottom left triangle.
//                        vertArray.push_back(Vector3(xOffset + _voxelWidth, yOffset + _voxelHeight, zOffset));
//                        vertArray.push_back(Vector3(xOffset,              yOffset + _voxelHeight, zOffset));
//                        vertArray.push_back(Vector3(xOffset + _voxelWidth, yOffset + _voxelHeight, neZPos ));
//                        // Top right triangle.
//                        vertArray.push_back(Vector3(xOffset + _voxelWidth, yOffset + _voxelHeight, neZPos ));
//                        vertArray.push_back(Vector3(xOffset,              yOffset + _voxelHeight, zOffset));
//                        vertArray.push_back(Vector3(xOffset,              yOffset + _voxelHeight, nwZPos ));
//                    }
//
//                    { // South Wall
//                        // Bottom left triangle.
//                        vertArray.push_back(Vector3(xOffset,              yOffset,               zOffset));
//                        vertArray.push_back(Vector3(xOffset + _voxelWidth, yOffset,               zOffset));
//                        vertArray.push_back(Vector3(xOffset,              yOffset,               swZPos ));
//                        // Top right triangle.
//                        vertArray.push_back(Vector3(xOffset,              yOffset,               swZPos ));
//                        vertArray.push_back(Vector3(xOffset + _voxelWidth, yOffset,               zOffset));
//                        vertArray.push_back(Vector3(xOffset + _voxelWidth, yOffset,               seZPos ));
//                    }
