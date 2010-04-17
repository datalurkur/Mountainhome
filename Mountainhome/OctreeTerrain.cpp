/*
 *  OctreeTerrain.cpp
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
#include "MHIndexedWorldModel.h"
#include "OctreeTerrain.h"

#define CACHE_SURFACE

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark OctreeTerrain definitions
//////////////////////////////////////////////////////////////////////////////////////////
OctreeTerrain::OctreeTerrain(int width, int height, int depth): _tileWidth(1.0), _tileHeight(1.0), _tileDepth(0.8), _surfaceCache(NULL) {
    _rootGroup = new TileGroup(Vector3(0, 0, 0), Vector3(width, height, depth), 0, 0);

#ifdef CACHE_SURFACE
    initCache();
#endif
}

OctreeTerrain::~OctreeTerrain() {
#ifdef CACHE_SURFACE
    clearCache();
#endif
    delete _rootGroup;
    clear_list(_models);
}

TileGroup::TileData OctreeTerrain::getTile(int x, int y, int z) {
    return _rootGroup->getTile(Vector3(x, y, z));
}

void OctreeTerrain::setTile(int x, int y, int z, TileGroup::TileData type) {
#ifdef CACHE_SURFACE
    int cached;
    if(getCacheValue(x, y, &cached)) {
        if(type==0) {
            // This could possibly lower our surface value
            if(z <= cached && z > 0) {
                setCacheValue(x, y, z-1);
            }
            else if(z <= 1) {
                //Info("Cannot cache bottomed-out surface!");
                setCacheValue(x, y, 0);
            }
        }
        else {
            // This could possibly raise our surface value
            if(z > cached) {
                setCacheValue(x, y, z);
            }
        }
    }
    else {
        setCacheValue(x, y, z);
    }
#endif
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

#ifdef CACHE_SURFACE
    int cached;
    if(getCacheValue(nX, nY, &cached)) {
        return cached;
    }
#endif
    
    return _rootGroup->getSurfaceLevel(Vector2(nX, nY));
}

void OctreeTerrain::clear() {
    _rootGroup->clearChildren();
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
    std::vector<Vector3> vertsArray;
    std::vector<Vector2> texCoordsArray;

    // Nuke everything already in place! The Scene and any models we put in it.
    scene->removeWorldObjects();
    clear_list(_models);

    // Build the vertex array
    for (int x = 0; x <= getWidth(); x++) {
        for (int y = 0; y <= getHeight(); y++) {
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
    Model *model = new MHIndexedWorldModel(indices, indexCount, vertices, normals, texCoords, vertexCount);
    _models.push_back(model);

    // Create the entity and add it to the scene.
    Entity *entity = scene->createEntity(model, "world");
    entity->setMaterial(mManager->getCachedResource("grass"));
    scene->getRootNode()->attach(entity);
}

void OctreeTerrain::save(std::string filename) {
    /* FORMAT DESCRIPTOR
        - # Tilegroups  (int      )
        - TileGroups
            - Position  (float [3])
            - Dimension (int   [3])
            - Type      (TileGroup::TileData)
            Note : Tilegroups are ordered heirarchically
                 : TileGroup, TileGroup's children (recursive)
    */

    File *tFile = FileSystem::GetFile(filename);
    tFile->open();
    if(!tFile->isOpen()) {
        Error("Filesystem failed to open terrain save file");
        return;
    }

    Info("Saving terrain data to " << filename);

    // Write a placeholder for the number of tilegroups
    int numGroups = 0;
    long groupCountPos = tFile->position();
    tFile->write(&numGroups, sizeof(int));

    // Recursively write the tilegroups, tracking the number written along the way
    numGroups = _rootGroup->write(tFile);

    // Rewind and write the actual number of tilegroups
    tFile->seek(groupCountPos);
    tFile->write(&numGroups, sizeof(int));

    // Finish and close
    tFile->close();
}

void OctreeTerrain::load(std::string filename) {
    // First, clear out any existing data in the terrain
    if(_rootGroup) {
        delete _rootGroup;
        _rootGroup = NULL;
    }

    File *tFile = FileSystem::GetFile(filename);

    if(!tFile->exists()) {
        Error("Terrain file " << filename << " does not exist.");
        return;
    }

    tFile->open();

    Info("Loading terrain data from " << filename);

    // Read in the number of tilegroups to follow
    int numGroups;
    tFile->read(&numGroups, sizeof(int));

    for (int c = 0; c < numGroups; c++) {
        // Read in a tilegroup
        Vector3 pos, dims;
        TileGroup::TileData type;
        tFile->read(&pos,  sizeof(int)*3);
        tFile->read(&dims, sizeof(int)*3);
        tFile->read(&type, sizeof(TileGroup::TileData));

        // Info("Read in an octant (" << type << ") at " << pos << " of size " << dims);

        if (!_rootGroup) {
            _rootGroup = new TileGroup(pos, dims, type, NULL);
#ifdef CACHE_SURFACE
            initCache();
#endif
        } else {
            _rootGroup->addOctant(pos, dims, type);
        }
    }

    tFile->close();
}

void OctreeTerrain::initCache() {
    int w = getWidth(),
        h = getHeight();

    clearCache();

    // Allocate memory (zeroed)
    _surfaceCache = (int**)calloc(w*h, sizeof(int*));
}

int OctreeTerrain::cacheIndex(int x, int y) {
    return (getWidth()*y)+x;
}

bool OctreeTerrain::getCacheValue(int x, int y, int *value) {
    int cIndex = cacheIndex(x,y);

    if(_surfaceCache[cIndex]) {
        *value = *_surfaceCache[cIndex];
        return true;
    }

    return false;
}

void OctreeTerrain::setCacheValue(int x, int y, int value) {
    int cIndex = cacheIndex(x,y);

    if(_surfaceCache[cIndex]) {
        *_surfaceCache[cIndex] = value;
    }
    else {
        _surfaceCache[cIndex] = new int(value);
    }
}

void OctreeTerrain::clearCache() {
    if(_surfaceCache) {
        int w = getWidth(),
            h = getHeight();

        for(int c=0; c<(w*h); c++) {
            if(_surfaceCache[c]) {
                delete _surfaceCache[c];
                _surfaceCache[c] = NULL;
            }
        }
        free(_surfaceCache);
        _surfaceCache = NULL;
    }
}
