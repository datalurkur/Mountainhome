/*
 *  Terrain.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _Terrain_H_
#define _Terrain_H_
#include <Base/Vector.h>
#include <vector>

#include "VoxelPalette.h"

class TerrainChunk;
class OctreeSceneManager;
class TerrainPalette;
class VoxelGrid;

class Terrain {
public:
    /*! The terrain is broken into chunks, for ease of processing. This represents the
     *  size of those chunks. The size of chunks in the terrain. */
    static const int ChunkSize = 25;

public:
    Terrain(int width, int height, int depth, OctreeSceneManager *sceneManager);
    ~Terrain();

    VoxelPalette * getPalette();

    PaletteIndex getPaletteIndex(int x, int y, int z);

    void setPaletteIndex(int x, int y, int z, PaletteIndex type);

    const ParameterData & getVoxelParameter(int x, int y, int z, ParameterID id);

    const Voxel * getVoxel(int x, int y, int z);

    int getSurfaceLevel(int x, int y);

    /*! Resets the terrain to be entirely empty and destroys all terrain geometry. */
    void clear();



    /*! Saves the terrain to the given file. */
    void save(const std::string &filename);

    /*! Loads the terrain to the given file and generates all geometry. */
    void load(const std::string &filename);

    /*! Causes all geometry in the world to generate itself. This is an expensive
     *  operation and should generally only be done as a precursor to initial
     *  rendering. */
    void populate();

    bool isOutOfBounds(Vector3 pos);

    bool isOutOfBounds(int x, int y, int z);



    /*! Gets the width of the entire world. */
    int getWidth();

    /*! Gets the height of the entire world. */
    int getHeight();

    /*! Gets the depth of the entire world. */
    int getDepth();

    void setPolyReduction(bool val);
    void setAutoUpdate   (bool val);

private:
    /*! Retrieves the TerrainChunk for the giving x/y/z world location and converts the
     *  world coordinates into chunk local coordinates. It one does not already exist, it
     *  will be created and added to the chunk map. */
    TerrainChunk *findOrCreateChunk(int *x, int *y, int *z);

private:
    /*! The type used to lookup chunks in the ChunkLookupMap. */
    typedef int ChunkIndex;

    /*! The sparse map of currently loaded chunks. */
    typedef std::map<ChunkIndex, TerrainChunk*> ChunkLookupMap;

    /*! The number of bits assigned to each dimension in the ChunkIndex type. */
    static const size_t BitsPerDim = sizeof(ChunkIndex) * 8 / 3;

private:
    ChunkLookupMap _chunks;

    int _width, _height, _depth;

    bool _polyReduction;

    bool _autoUpdate;

    OctreeSceneManager *_sceneManager;

    VoxelPalette *_voxelPalette;

};

#endif
