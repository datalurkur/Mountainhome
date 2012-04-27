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


#define GET_CHUNK_INDEX(x, y, z) ((((x) / ChunkSize) << (BitsPerDim * 2)) | (((y) / ChunkSize) << BitsPerDim) | ((z) / ChunkSize))

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
    _grid = new MatrixVoxelGrid(_width, _height, _depth);
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

TerrainChunk *Terrain::findOrCreateChunkNode(int x, int y, int z) {
    TerrainChunk *retValue = NULL;

    ChunkIndex chunkIndex = GET_CHUNK_INDEX(x, y, z);

    ChunkLookupMap::iterator itr = _chunks.find(chunkIndex);

    if (itr == _chunks.end()) {
        retValue = new TerrainChunk(
            x / ChunkSize, y / ChunkSize, z / ChunkSize,
            _grid, _voxelPalette);

        _sceneManager->addNode(retValue);
        _chunks[chunkIndex] = retValue;
    } else {
        retValue = itr->second;
    }

    return retValue;
}

void Terrain::markDirty(int x, int y, int z, PaletteIndex type) {
    ASSERT(type != VoxelPalette::EmptyVoxel);
    // Note that we always need to find the proper node, to make sure they're created.
    // Only mark the node dirty if we're setup to auto update, though.
    TerrainChunk *node = findOrCreateChunkNode(x, y, z);
    if (_autoUpdate) {
        node->markDirty(type);
    }
}

PaletteIndex Terrain::getPaletteIndex(int x, int y, int z) {
    return _grid->getPaletteIndex(x, y, z);
}

void Terrain::setPaletteIndex(int x, int y, int z, PaletteIndex newType) {
    PaletteIndex oldType = getPaletteIndex(x, y, z);

    if (oldType != newType) {
        _grid->setPaletteIndex(x, y, z, newType);

        // Always update the oldType, regardless of whether or not we're adding, removing,
        // or changing the voxel type.
        if (oldType != VoxelPalette::EmptyVoxel) { markDirty(x, y, z, oldType); }

        if (newType == VoxelPalette::EmptyVoxel) {
            // If the voxel is newly empty, we need to update any non-empty adjacent
            // type/nodes if the adjacent space is in a different node or of a different
            // type than oldType.

#define MARK_IF_NOT_EMPTY(x, y, z, otherType, diffNode) do { \
        if ((x) >= 0 && (x) < _width  && \
            (y) >= 0 && (y) < _height && \
            (z) >= 0 && (z) < _depth) \
        { \
            PaletteIndex type = getPaletteIndex((x), (y), (z)); \
            if (type != VoxelPalette::EmptyVoxel && ((diffNode) || type != (otherType))) { \
                markDirty((x), (y), (z), type); \
            } \
        } \
    } while (0)

            MARK_IF_NOT_EMPTY(x - 1, y, z, oldType, x % ChunkSize == 0            );
            MARK_IF_NOT_EMPTY(x + 1, y, z, oldType, x % ChunkSize == ChunkSize - 1);
            MARK_IF_NOT_EMPTY(x, y - 1, z, oldType, y % ChunkSize == 0            );
            MARK_IF_NOT_EMPTY(x, y + 1, z, oldType, y % ChunkSize == ChunkSize - 1);
            MARK_IF_NOT_EMPTY(x, y, z - 1, oldType, z % ChunkSize == 0            );
            MARK_IF_NOT_EMPTY(x, y, z + 1, oldType, z % ChunkSize == ChunkSize - 1);

#undef MARK_IF_NOT_EMPTY
        } else {
            // Always update the new type at the current location and update any
            // non-empty, adjacent type/nodes if we're filling in an empty spot.
            markDirty(x, y, z, newType);

            if (oldType == VoxelPalette::EmptyVoxel) {
#define MARK_IF_NOT_EMPTY(x, y, z) do { \
        if ((x) >= 0 && (x) < _width  && \
            (y) >= 0 && (y) < _height && \
            (z) >= 0 && (z) < _depth) \
        { \
            PaletteIndex type = getPaletteIndex((x), (y), (z)); \
            if (type != VoxelPalette::EmptyVoxel) { markDirty((x), (y), (z), type); } \
        } \
    } while (0)

                MARK_IF_NOT_EMPTY(x - 1, y, z);
                MARK_IF_NOT_EMPTY(x + 1, y, z);
                MARK_IF_NOT_EMPTY(x, y - 1, z);
                MARK_IF_NOT_EMPTY(x, y + 1, z);
                MARK_IF_NOT_EMPTY(x, y, z - 1);
                MARK_IF_NOT_EMPTY(x, y, z + 1);

#undef MARK_IF_NOT_EMPTY
            }
        }
    }
}

int Terrain::getSurfaceLevel(int x, int y) {
    return _grid->getSurfaceLevel(x, y);
}

int Terrain::getEmptyRanges(int x, int y, std::vector<std::pair<int,int> > &ranges) {
    return _grid->getEmptyRanges(x, y, ranges);
}

int Terrain::getFilledRanges(int x, int y, std::vector<std::pair<int,int> > &ranges) {
    return _grid->getFilledRanges(x, y, ranges);
}

void Terrain::clear() {
    ChunkLookupMap::iterator itr = _chunks.begin();
    for (; itr != _chunks.end(); itr++) {
        _sceneManager->deleteNode<TerrainChunk>(itr->second->getName());
    }

    _grid->clear();
}
    
void Terrain::save(const std::string &filename) {
    File *file = FileSystem::GetFile(filename, IOTarget::Write);
    _grid->save(file);
    delete file;
}

void Terrain::load(const std::string &filename) {
    File *file = FileSystem::GetFile(filename, IOTarget::Read);
    _grid->load(file);
    delete file;

    populate();
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
