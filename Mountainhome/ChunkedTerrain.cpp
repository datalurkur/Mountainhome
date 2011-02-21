/*
 *  ChunkedTerrain.cpp
 *  Mountainhome
 *
 *  Created by loch on 7/12/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "ChunkedTerrain.h"

#include <Base/File.h>
#include <Base/Timer.h>
#include <Base/FileSystem.h>
#include <Content/Content.h>
#include <Engine/Entity.h>

#include "ChunkedTerrain.h"
#include "MatrixTileGrid.h"
#include "TerrainChunkNode.h"
#include "OctreeSceneManager.h"

#define GET_CHUNK_INDEX(x, y, z) ((((x) / ChunkSize) << (BitsPerDim * 2)) | (((y) / ChunkSize) << BitsPerDim) | ((z) / ChunkSize))

// TODO: Remove empty TerrainChunkNodes.

ChunkedTerrain::ChunkedTerrain(
    int width, int height, int depth,
    OctreeSceneManager *scene
):
    MHTerrain(width, height, depth, scene)
{
    _grid = new MatrixTileGrid(width, height, depth);
    clear();
}

ChunkedTerrain::~ChunkedTerrain() {
    clear();
    delete _grid;
}

TerrainChunkNode *ChunkedTerrain::findOrCreateChunkNode(int x, int y, int z) {
    TerrainChunkNode *retValue = NULL;

    IndexType chunkIndex = GET_CHUNK_INDEX(x, y, z);

    ChunkLookupMap::iterator itr = _chunks.find(chunkIndex);

    if (itr == _chunks.end()) {
        retValue = new TerrainChunkNode(
            x / ChunkSize, y / ChunkSize, z / ChunkSize,
            _tilePalette, _grid);

        _sceneManager->addNode(retValue);
        _chunks[chunkIndex] = retValue;
    } else {
        retValue = itr->second;
    }

    return retValue;
}

void ChunkedTerrain::markDirty(int x, int y, int z, PaletteIndex type) {
    ASSERT(type != TILE_EMPTY);
    TerrainChunkNode *node = findOrCreateChunkNode(x, y, z);
    node->markDirty(type);
}

PaletteIndex ChunkedTerrain::getPaletteIndex(int x, int y, int z) {
    return _grid->getPaletteIndex(x, y, z);
}

void ChunkedTerrain::setPaletteIndex(int x, int y, int z, PaletteIndex newType) {
    PaletteIndex oldType = getPaletteIndex(x, y, z);

    if (oldType != newType) {
        _grid->setPaletteIndex(x, y, z, newType);

        // Always update the oldType, regardless of whether or not we're adding, removing,
        // or changing the tile type.
        if (oldType != TILE_EMPTY) { markDirty(x, y, z, oldType); }

        if (newType == TILE_EMPTY) {
            // If the tile is newly empty, we need to update any non-empty adjacent
            // type/nodes if the adjacent space is in a different node or of a different
            // type than oldType.

#define MARK_IF_NOT_EMPTY(x, y, z, otherType, diffNode) do { \
        if ((x) >= 0 && (x) < _width  && \
            (y) >= 0 && (y) < _height && \
            (z) >= 0 && (z) < _depth) \
        { \
            PaletteIndex type = getPaletteIndex((x), (y), (z)); \
            if (type != TILE_EMPTY && ((diffNode) || type != (otherType))) { \
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

            if (oldType == TILE_EMPTY) {
#define MARK_IF_NOT_EMPTY(x, y, z) do { \
        if ((x) >= 0 && (x) < _width  && \
            (y) >= 0 && (y) < _height && \
            (z) >= 0 && (z) < _depth) \
        { \
            PaletteIndex type = getPaletteIndex((x), (y), (z)); \
            if (type != TILE_EMPTY) { markDirty((x), (y), (z), type); } \
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

int ChunkedTerrain::getSurfaceLevel(int x, int y) {
    return _grid->getSurfaceLevel(x, y);
}

int ChunkedTerrain::getEmptyRanges(int x, int y, std::vector<std::pair<int,int> > &ranges) {
    return _grid->getEmptyRanges(x, y, ranges);
}

int ChunkedTerrain::getFilledRanges(int x, int y, std::vector<std::pair<int,int> > &ranges) {
    return _grid->getFilledRanges(x, y, ranges);
}

void ChunkedTerrain::clear() {
    ChunkLookupMap::iterator itr = _chunks.begin();
    for (; itr != _chunks.end(); itr++) {
        _sceneManager->deleteNode<TerrainChunkNode>(itr->second->getName());
    }

    _grid->clear();
}
    
void ChunkedTerrain::save(const std::string &filename) {
    File *file = FileSystem::GetFile(filename, IOTarget::Write);
    _grid->save(file);
    delete file;
}

void ChunkedTerrain::load(const std::string &filename) {
    File *file = FileSystem::GetFile(filename, IOTarget::Read);
    _grid->load(file);
    delete file;

    populate();
}

void ChunkedTerrain::populate() {
    Timer t;

    if (_polyReduction) { t.start(); }

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
