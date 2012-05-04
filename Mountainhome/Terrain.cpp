/*
 *  Terrain.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "Terrain.h"
#include "OctreeSceneManager.h"
#include "VoxelPalette.h"

#include <Base/File.h>
#include <Base/Timer.h>
#include <Base/FileSystem.h>
#include <Content/Content.h>
#include <Engine/Entity.h>

#include "TerrainChunk.h"
#include "MatrixVoxelGrid.h"

// TODO: Remove empty TerrainChunks.

Terrain::Terrain(
    int width,
    int height,
    int depth,
    OctreeSceneManager *sceneManager
):
    _width(width),
    _height(height),
    _depth(depth),
    _polyReduction(false),
    _autoUpdate(false),
    _sceneManager(sceneManager)
{
    _voxelPalette = new VoxelPalette();
    clear();
}

Terrain::~Terrain() {
    clear();
    delete _voxelPalette;
}

VoxelPalette * Terrain::getPalette() { return _voxelPalette; }

const ParameterData & Terrain::getVoxelParameter(int x, int y, int z, ParameterID id) {
    return getVoxel(x, y, z)->getParameter(id);
}

const Voxel * Terrain::getVoxel(int x, int y, int z) {
    return _voxelPalette->getVoxelForIndex(getPaletteIndex(x, y, z));
}

bool Terrain::isOutOfBounds(Vector3 pos) { return isOutOfBounds(pos[0], pos[1], pos[2]); }
bool Terrain::isOutOfBounds(int x, int y, int z) {
    return (x < 0 || x >= _width  ||
            y < 0 || y >= _height ||
            z < 0 || z >= _depth);
}

int Terrain::getWidth()  { return _width;  }
int Terrain::getHeight() { return _height; }
int Terrain::getDepth()  { return _depth;  }

void Terrain::setPolyReduction(bool val) { _polyReduction = val; }
void Terrain::setAutoUpdate   (bool val) { _autoUpdate    = val; }

TerrainChunk *Terrain::findOrCreateChunk(int *x, int *y, int *z) {
    TerrainChunk *retValue = NULL;

    // Find the chunk index for the input coords, and convert them to local chunk coords.
    int chunkX = *x / ChunkSize; *x -= chunkX * ChunkSize;
    int chunkY = *y / ChunkSize; *y -= chunkY * ChunkSize;
    int chunkZ = *z / ChunkSize; *z -= chunkZ * ChunkSize;

    // Compact the chunk coords into a chunk index.
    ChunkIndex chunkIndex =
        (chunkX << (BitsPerDim * 2)) |
        (chunkY << (BitsPerDim * 1)) |
        (chunkZ << (BitsPerDim * 0));

    // Lookup the chunk. Create it, if it doesn't exist.
    ChunkLookupMap::iterator itr = _chunks.find(chunkIndex);

    if (itr == _chunks.end()) {
        retValue = new TerrainChunk(chunkX, chunkY, chunkZ, this);

        _sceneManager->addNode(retValue);
        _chunks[chunkIndex] = retValue;
    } else {
        retValue = itr->second;
    }

    return retValue;
}

PaletteIndex Terrain::getPaletteIndex(int x, int y, int z) {
    TerrainChunk *chunk = findOrCreateChunk(&x, &y, &z);
    return chunk->getLocalGrid()->getPaletteIndex(x, y, z);
}

void Terrain::setPaletteIndex(int x, int y, int z, PaletteIndex newType) {
    int localX = x, localY = y, localZ = z;
    TerrainChunk *chunk = findOrCreateChunk(&localX, &localY, &localZ);
    PaletteIndex oldType = chunk->getLocalGrid()->getPaletteIndex(localX, localY, localZ);

    if (oldType != newType) {
        chunk->getLocalGrid()->setPaletteIndex(localX, localY, localZ, newType);

        // If auto update is on, we need to mark chunks as dirty, to make sure they
        // regerate their geometry correctly.
        if (_autoUpdate) {
            chunk->markDirty(oldType);
            chunk->markDirty(newType);

            // If we're at a chunk border, we have slightly more work to do.
            if (localX == 0 || localX == ChunkSize - 1 ||
                localY == 0 || localY == ChunkSize - 1 ||
                localZ == 0 || localZ == ChunkSize - 1)
            {
                #define MARK_DIRTY(x, y, z) do { \
                    localX = (x), localY = (y), localZ = (z); \
                    if (localX >= 0 && localX < _width  && \
                        localY >= 0 && localY < _height && \
                        localZ >= 0 && localZ < _depth) \
                    { \
                        chunk = findOrCreateChunk(&localX, &localY, &localZ); \
                        PaletteIndex type = chunk->getLocalGrid()->getPaletteIndex(localX, localY, localZ); \
                        chunk->markDirty(type); \
                    } \
                } while (0)

                MARK_DIRTY(x - 1, y, z);
                MARK_DIRTY(x + 1, y, z);
                MARK_DIRTY(x, y - 1, z);
                MARK_DIRTY(x, y + 1, z);
                MARK_DIRTY(x, y, z - 1);
                MARK_DIRTY(x, y, z + 1);

                #undef MARK_DIRTY
            }
            else
            {
                #define MARK_DIRTY(x, y, z) do { \
                    PaletteIndex type = chunk->getLocalGrid()->getPaletteIndex((x), (y), (z)); \
                    if (type != newType && type != oldType && type != VoxelPalette::EmptyVoxel) { \
                        chunk->markDirty(type); \
                    } \
                } while (0)

                MARK_DIRTY(localX - 1, localY, localZ);
                MARK_DIRTY(localX + 1, localY, localZ);
                MARK_DIRTY(localX, localY - 1, localZ);
                MARK_DIRTY(localX, localY + 1, localZ);
                MARK_DIRTY(localX, localY, localZ - 1);
                MARK_DIRTY(localX, localY, localZ + 1);

                #undef MARK_DIRTY
            }
        }
    }
}

int Terrain::getSurfaceLevel(int x, int y) {
    for (int z = _depth - 1; z >= 0; z-=ChunkSize) {
        int localX = x, localY = y, localZ = z;
        TerrainChunk *chunk = findOrCreateChunk(&localX, &localY, &localZ);
        localZ = chunk->getLocalGrid()->getSurfaceLevel(localX, localY);

        if (localZ >= 0) {
            return chunk->getZChunkIndex() * ChunkSize + localZ;
        }
    }
    return -1;
}

void Terrain::clear() {
    ChunkLookupMap::iterator itr = _chunks.begin();
    for (; itr != _chunks.end(); itr++) {
        _sceneManager->deleteNode<TerrainChunk>(itr->second->getName());
    }
}
    
void Terrain::save(const std::string &filename) {
    THROW(NotImplementedError, "Terrain::save not implemented to handle virtualization, yet.");
//    File *file = FileSystem::GetFile(filename, IOTarget::Write);
//    _grid->save(file);
//    delete file;
}

void Terrain::load(const std::string &filename) {
    THROW(NotImplementedError, "Terrain::load not implemented to handle virtualization, yet.");
//    File *file = FileSystem::GetFile(filename, IOTarget::Read);
//    _grid->load(file);
//    delete file;
//
//    populate();
}

void Terrain::populate() {
    Timer t;

    if (_polyReduction) { t.start(); }

    // Tell all chunks to populate and count their renderables for logging purposes.
    int count = 0;
    ChunkLookupMap::iterator itr = _chunks.begin();
    for (; itr != _chunks.end(); itr++) {
        count += itr->second->populate();
    }

    if (_polyReduction) {
        t.stop();
        Info("World population stats:");
        Info("    World size:          " << _width << "x" << _height << "x" << _depth);
        Info("    Renderables updated: " << count);
        Info("    Time (seconds):      " << t.seconds()  << " (" << (t.seconds()  / count) << " per chunk)");
        Info("    Time (mseconds):     " << t.mseconds() << " (" << (t.mseconds() / count) << " per chunk)");
    }
}
