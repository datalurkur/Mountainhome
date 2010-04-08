/*
 *  OctreeTerrain.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHIndexedWorldModel.h"
#include "OctreeTerrain.h"

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark OctreeTerrain definitions
//////////////////////////////////////////////////////////////////////////////////////////
OctreeTerrain::OctreeTerrain(int width, int height, int depth): _tileWidth(1.0), _tileHeight(1.0), _tileDepth(0.8) {
    _rootGroup = new TileGroup<short>(Vector3(0, 0, 0), Vector3(width, height, depth), 0, 0);
}

OctreeTerrain::~OctreeTerrain() {}

short OctreeTerrain::getTile(int x, int y, int z) {
    return _rootGroup->getTile(Vector3(x, y, z));
}

void OctreeTerrain::setTile(int x, int y, int z, short type) {
    _rootGroup->setTile(Vector3(x, y, z), type);
}

int OctreeTerrain::getSurfaceLevel(int x, int y) {
    int nX = x,
        nY = y;
    Vector3 dims = _rootGroup->getDims();
    
    if(nX < 0) { nX = 0; }
    else if(nX >= dims[0]) { nX = dims[0]-1; }
    if(nY < 0) { nY = 0; }
    else if(nY >= dims[1]) { nY = dims[1]-1; }
    
    return _rootGroup->getSurfaceLevel(Vector2(nX, nY));
}

int OctreeTerrain::getHeight() {
    return _rootGroup->getDims()[0];
}

int OctreeTerrain::getWidth() {
    return _rootGroup->getDims()[1];
}

int OctreeTerrain::getDepth() {
    return _rootGroup->getDims()[2];
}

void OctreeTerrain::populate(OctreeSceneManager *scene, MaterialManager *mManager) {
    int x, y, i, j;
    std::vector<Vector3> vertsArray;
    std::vector<Vector2> texCoordsArray;

    // Build the vertex array
    for(x=0; x <= getWidth(); x++) {
        for(int y=0; y <= getHeight(); y++) {
            int z = getSurfaceLevel(x, y);
            vertsArray.push_back(Vector3(x * _tileWidth, y * _tileHeight, z * _tileDepth));
            texCoordsArray.push_back(Vector2(x, y));
        }
    }

    int vertexCount = vertsArray.size();
    Vector3 *vertices = vector_to_array(vertsArray);
    Vector2 *texCoords = vector_to_array(texCoordsArray);

    // Build the index array
    std::vector<unsigned int> indexArray;
    for(x=0; x < getWidth(); x++) {
        for(y=0; y < getHeight(); y++) {
            #define TRANSLATE(x, y) ((x) * (getWidth() + 1) + (y))
            // SW Triangle
            indexArray.push_back(TRANSLATE(x,   y  ));
            indexArray.push_back(TRANSLATE(x+1, y  ));
            indexArray.push_back(TRANSLATE(x,   y+1));
            // NE Triangle
            indexArray.push_back(TRANSLATE(x,   y+1));
            indexArray.push_back(TRANSLATE(x+1, y  ));
            indexArray.push_back(TRANSLATE(x+1, y+1));
            #undef TRANSLATE
        }
    }

    int indexCount = indexArray.size();
    unsigned int *indices = vector_to_array(indexArray);

    // Calculate the normal for each vertex in the world by averaging the normal of all of
    // the faces a vertex is shared between. SPECIAL NOTE: In this particular case, we
    // know that all of the vertices will be attached to indices. In the generic case,
    // we don't, however, and should ensure we don't normalize bogus memory in the second
    // step of this process.
    Vector3 *normals = new Vector3[vertexCount];

    for (i = 0; i < indexCount; i+=3) {
        Vector3 one = vertices[indexArray[i+1]] - vertices[indexArray[i+0]];
        Vector3 two = vertices[indexArray[i+2]] - vertices[indexArray[i+1]];
        Vector3 polyNormal = one.crossProduct(two);
        polyNormal.normalize();

        for (j = 0; j < 3; j++) {
            normals[indexArray[i+j]] += polyNormal;
        }
    }

    for (i = 0; i < vertexCount; i++) {
        normals[i].normalize();
    }

    Entity *entity = scene->createEntity(new MHIndexedWorldModel(indices, indexCount, vertices, normals, texCoords, vertexCount), "world");
    entity->setMaterial(mManager->getCachedResource("grass"));
    scene->getRootNode()->attach(entity);
}

