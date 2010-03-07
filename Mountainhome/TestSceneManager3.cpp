/*
 *  TestSceneManager3.cpp
 *  Mountainhome
 *
 *  Created by loch on 3/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "TestSceneManager3.h"

#include <Render/MaterialManager.h>
#include <Render/Entity.h>
#include <Render/Model.h>
#include <Render/Node.h>

#include "TestSceneManager3.h"
#include "MHWorld.h"

TestSceneManager3::TestSceneManager3(MHWorld *world): MHSceneManager(world) {}

TestSceneManager3::~TestSceneManager3() {}

int TestSceneManager3::getLowestAtCorner(int x, int y) const {
    int min = _world->getHeight();
    MHWorld::Tile *t = NULL;

    // Base off of south west corner.
    if (t = _world->getTopTile(x,     y    )) { min = std::min(min, t->z); }
    if (t = _world->getTopTile(x - 1, y    )) { min = std::min(min, t->z); }
    if (t = _world->getTopTile(x,     y - 1)) { min = std::min(min, t->z); }
    if (t = _world->getTopTile(x - 1, y - 1)) { min = std::min(min, t->z); }
    return min;
}

Vector3 TestSceneManager3::getVectorForCorner(int x, int y) const {
    return Vector3(x * _tileWidth, y * _tileHeight, getLowestAtCorner(x, y) * _tileWidth);
}

void TestSceneManager3::populate() {
    // Build the vertex array by choosing the lowest z value of the 4 top level tiles
    // around every corner in the map.
    std::vector<Vector3> vertsArray;
    for (int x = 0; x < _world->getWidth(); x++) {
        for (int y = 0; y < _world->getHeight(); y++) {
            // Bottom left triangle.
            vertsArray.push_back(getVectorForCorner(x + 0, y + 0));
            vertsArray.push_back(getVectorForCorner(x + 1, y + 0));
            vertsArray.push_back(getVectorForCorner(x + 0, y + 1));

            // Top right triangle.
            vertsArray.push_back(getVectorForCorner(x + 0, y + 1));
            vertsArray.push_back(getVectorForCorner(x + 1, y + 0));
            vertsArray.push_back(getVectorForCorner(x + 1, y + 1));
        }    
    }

    Vector3 *verts = new Vector3[vertsArray.size()];
    Vector3 *norms = new Vector3[vertsArray.size()];
    for (int i = 0; i < vertsArray.size(); i+=3) {
		Vector3 one = vertsArray[i+1] - vertsArray[i+0];
		Vector3 two = vertsArray[i+2] - vertsArray[i+1];
		Vector3 normal = one.crossProduct(two);
        normal.normalize();

        for (int j = 0; j < 3; j++) {
            verts[i+j] = vertsArray[i+j];
            norms[i+j] = normal;
        }
    }

    Entity *entity = createEntity(new WorldEntity(verts, norms, vertsArray.size()), "world");
    entity->setMaterial(MaterialManager::Get()->loadResource("white"));
    getRootNode()->attach(entity);
}
