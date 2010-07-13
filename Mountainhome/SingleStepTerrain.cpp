/*
 *  SingleStepTerrain.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include <Base/FileSystem.h>
#include <Render/MaterialManager.h>
#include <Render/Entity.h>
#include <Render/Node.h>

#include "OctreeSceneManager.h"
#include "MHIndexedModel.h"
#include "MHReducedModel.h"
#include "SingleStepTerrain.h"

// XXXBMW Broken when I moved the loading code into the TileGrid. Surface cache should probably be moved elsewhere, anyways.
// #define CACHE_SURFACE

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark SingleStepTerrain definitions
//////////////////////////////////////////////////////////////////////////////////////////
SingleStepTerrain::SingleStepTerrain(int width, int height, int depth,
OctreeSceneManager *scene, MaterialManager *manager): _tileWidth(1.0), _tileHeight(1.0),
_tileDepth(1.0), _surfaceCache(NULL), _sceneManager(scene), _materialManager(manager) {
    _rootGroup = new OctreeTileGrid(width, height, depth, Vector3(0, 0, 0), TILE_EMPTY, NULL);

#ifdef CACHE_SURFACE
    initCache();
#endif
}

SingleStepTerrain::~SingleStepTerrain() {
#ifdef CACHE_SURFACE
    clearCache();
#endif
    delete _rootGroup;
    clear_list(_models);
}

TileType SingleStepTerrain::getTile(int x, int y, int z) {
    return _rootGroup->getTile(x, y, z);
}

void SingleStepTerrain::setTile(int x, int y, int z, TileType type) {
#ifdef CACHE_SURFACE
    int cached;
    if (getCacheValue(x, y, &cached)) {
        if (type == 0) {
            // This could possibly lower our surface value
            if (z <= cached && z > 0) {
                setCacheValue(x, y, z - 1);
            } else if(z <= 1) {
                //Info("Cannot cache bottomed-out surface!");
                setCacheValue(x, y, 0);
            }
        } else {
            // This could possibly raise our surface value
            if(z > cached) {
                setCacheValue(x, y, z);
            }
        }
    } else {
        setCacheValue(x, y, z);
    }
#endif
    _rootGroup->setTile(x, y, z, type);
}

int SingleStepTerrain::getSurfaceLevel(int x, int y) {
    int nX = x,
        nY = y;
    Vector3 dims = _rootGroup->getDimensions();
    
    if(nX < 0) { nX = 0; }
    else if(nX >= dims[0]) { nX = dims[0]-1; }
    if(nY < 0) { nY = 0; }
    else if(nY >= dims[1]) { nY = dims[1]-1; }

#ifdef CACHE_SURFACE
    int cached;
    if(getCacheValue(nX, nY, &cached)) {
        return cached;
    }
#endif

    return _rootGroup->getSurfaceLevel(nX, nY);
}

void SingleStepTerrain::clear() {
    _rootGroup->clear();
}

int SingleStepTerrain::getHeight() {
    return _rootGroup->getHeight();
}

int SingleStepTerrain::getWidth() {
    return _rootGroup->getWidth();
}

int SingleStepTerrain::getDepth() {
    return _rootGroup->getDepth();
}

void SingleStepTerrain::populate(bool final) {
    std::vector<Vector3> vertsArray;
    std::vector<Vector2> texCoordsArray;

    // Nuke everything already in place! The Scene and any models we put in it.
    _sceneManager->removeEntity("terrain");
    clear_list(_models);

    // Build the vertex array
    for (int x = 0; x <= getWidth(); x++) {
        for (int y = 0; y <= getHeight(); y++) {
            int z = getSurfaceLevel(x, y);
            vertsArray.push_back(Vector3(x * _tileWidth, y * _tileHeight, z * _tileDepth));
            texCoordsArray.push_back(Vector2(0.1*x, 0.1*y));
        }
    }

    int vertexCount = vertsArray.size();
    Vector3 *vertices = vector_to_array(vertsArray);
    Vector2 *texCoords = vector_to_array(texCoordsArray);

    // Build the index array
    std::vector<unsigned int> indexArray;
    for (int x = 0; x < getWidth(); x++) {
        for (int y = 0; y < getHeight(); y++) {
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
    memset(normals, 0, sizeof(Vector3) * vertexCount);

    for (int i = 0; i < indexCount; i+=3) {
        Vector3 one = vertices[indexArray[i+1]] - vertices[indexArray[i+0]];
        Vector3 two = vertices[indexArray[i+2]] - vertices[indexArray[i+1]];
        Vector3 polyNormal = one.crossProduct(two);
        polyNormal.normalize();

        for (int j = 0; j < 3; j++) {
            normals[indexArray[i+j]] += polyNormal;
        }
    }

    for (int i = 0; i < vertexCount; i++) {
        normals[i].normalize();
    }

    // Create the model and store it for later cleanup.
    Model *model;
    if (final && getWidth() <= 33 && getHeight() <= 33 && getDepth() <= 17) {
        model = new MHReducedModel(indices, indexCount, vertices, normals, texCoords, vertexCount);
    } else {
        model = new MHIndexedModel(indices, indexCount, vertices, normals, texCoords, vertexCount);
    }

    _models.push_back(model);

    // Create the entity and add it to the scene.
    Entity *entity = _sceneManager->createEntity(model, "terrain");
    entity->setMaterial(_materialManager->getCachedResource("grass"));
}

void SingleStepTerrain::save(std::string filename) {
    File *file = FileSystem::GetFile(filename, IOTarget::Write);
    _rootGroup->save(file);
    delete file;
}

void SingleStepTerrain::load(std::string filename) {
    File *file = FileSystem::GetFile(filename, IOTarget::Read);
    _rootGroup->load(file);
    delete file;
}

void SingleStepTerrain::initCache() {
    int w = getWidth(),
        h = getHeight();

    clearCache();

    // Allocate memory (zeroed)
    _surfaceCache = (int**)calloc(w*h, sizeof(int*));
}

int SingleStepTerrain::cacheIndex(int x, int y) {
    return (getWidth() * y) + x;
}

bool SingleStepTerrain::getCacheValue(int x, int y, int *value) {
    int cIndex = cacheIndex(x,y);

    if(_surfaceCache[cIndex]) {
        *value = *_surfaceCache[cIndex];
        return true;
    }

    return false;
}

void SingleStepTerrain::setCacheValue(int x, int y, int value) {
    int cIndex = cacheIndex(x,y);

    if(_surfaceCache[cIndex]) {
        *_surfaceCache[cIndex] = value;
    }
    else {
        _surfaceCache[cIndex] = new int(value);
    }
}

void SingleStepTerrain::clearCache() {
    if(_surfaceCache) {
        int size = getWidth() * getHeight();

        for(int c = 0; c < size; c++) {
            if (_surfaceCache[c]) {
                delete _surfaceCache[c];
                _surfaceCache[c] = NULL;
            }
        }

        free(_surfaceCache);
        _surfaceCache = NULL;
    }
}
