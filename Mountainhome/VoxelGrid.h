/*
 *  VoxelGrid.h
 *  Mountainhome
 *
 *  Created by loch on 7/9/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _TILEGRID_H_
#define _TILEGRID_H_
#include "VoxelPalette.h"
#include <Base/Vector.h>
#include <vector>

class IOTarget;

class VoxelGrid {
public:
    /*! Creates a new Voxel grid of the given size. */
    VoxelGrid(int width, int height, int depth):
        _width(width), _height(height), _depth(depth) {}

    /*! D'tor */
    virtual ~VoxelGrid() {}

    /*! Gets the dimensions of this group. */
    Vector3 getDimensions() { return Vector3(_width, _height, _depth); } 

    /*! Gets the width of the grid. */
    int getWidth () { return _width ; }

    /*! Gets the height of the grid. */
    int getHeight() { return _height; }

    /*! Gets the depth of the grid. */
    int getDepth () { return _depth ; }

    /*! Sets the voxel at the specified location. */
    virtual void setPaletteIndex(int x, int y, int z, PaletteIndex type) = 0;

    /*! Gets the voxel at the specified location. */
    virtual PaletteIndex getPaletteIndex(int x, int y, int z) = 0;

    /*! Gets the maximum, full z level at the given x/y location. */
    virtual int getSurfaceLevel(int x, int y) = 0;

    /*! Saves the VoxelGrid to the given IOTarget. */
    virtual void save(IOTarget *target) = 0;

    /*! Loads the VoxelGrid from the given IOTarget. */
    virtual void load(IOTarget *target) = 0;

    /*! Clears out the entire grid. */
    virtual void clear() = 0;

protected:
    int _width, _height, _depth;

};

#endif
