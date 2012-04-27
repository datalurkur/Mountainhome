/*
 *  HashVoxelGrid.h
 *  Mountainhome
 *
 *  Created by loch on 7/11/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _HashVoxelGrid_H_
#define _HashVoxelGrid_H_
#include "VoxelGrid.h"

class HashVoxelGrid : public VoxelGrid {
public:
    /*! Creates a new HashVoxelGrid of the given size. */
    HashVoxelGrid(int width, int height, int depth);

    /*! D'tor */
    virtual ~HashVoxelGrid();

    virtual void setPaletteIndex(int x, int y, int z, PaletteIndex type);
    virtual PaletteIndex getPaletteIndex(int x, int y, int z);

    /*! Gets the maximum, full z level at the given x/y location. */
    virtual int getSurfaceLevel(int x, int y);

    /*! Saves the VoxelGrid to the given IOTarget. */
    virtual void save(IOTarget *target);

    /*! Loads the VoxelGrid from the given IOTarget. */
    virtual void load(IOTarget *target);

    /*! Clears out the entire grid. */
    virtual void clear();

private:
    typedef int LookupType;

    // Convert the LookupType size from bytes to bits and split it into thirds (one for each dimension).
    static const size_t BitsPerDim = sizeof(LookupType) * 8 / 3;

private:
    std::map<LookupType, PaletteIndex> _voxelHash;
};

#endif
