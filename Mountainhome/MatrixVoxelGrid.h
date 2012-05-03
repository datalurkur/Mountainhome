/*
 *  MatrixVoxelGrid.h
 *  Mountainhome
 *
 *  Created by loch on 7/9/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MATRIXTILEGRID_H_
#define _MATRIXTILEGRID_H_
#include "VoxelGrid.h"

class MatrixVoxelGrid : public VoxelGrid {
public:

    /*! Creates a new MatrixVoxelGrid of the given size. */
    MatrixVoxelGrid(int width, int height, int depth);

    /*! D'tor */
    virtual ~MatrixVoxelGrid();

    /*! Sets the voxel type at the specified location. */
    virtual void setPaletteIndex(int x, int y, int z, PaletteIndex type);

    /*! Gets the voxel type at the specified location. */
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
    PaletteIndex *_voxelMatrix;
};

#endif
