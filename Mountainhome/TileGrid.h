/*
 *  TileGrid.h
 *  Mountainhome
 *
 *  Created by loch on 7/9/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _TILEGRID_H_
#define _TILEGRID_H_
#include "TilePalette.h"
#include <Base/Vector.h>
class IOTarget;

class TileGrid {
public:
    /*! Creates a new Tile grid of the given size. */
    TileGrid(int width, int height, int depth):
        _width(width), _height(height), _depth(depth) {}

    /*! D'tor */
    virtual ~TileGrid() {}

    /*! Gets the dimensions of this group. */
    Vector3 getDimensions() { return Vector3(_width, _height, _depth); } 

    /*! Gets the width of the grid. */
    int getWidth () { return _width ; }

    /*! Gets the height of the grid. */
    int getHeight() { return _height; }

    /*! Gets the depth of the grid. */
    int getDepth () { return _depth ; }

    /*! Sets the tile at the specified location. */
    virtual void setPaletteIndex(int x, int y, int z, PaletteIndex type) = 0;

    /*! Gets the tile at the specified location. */
    virtual PaletteIndex getPaletteIndex(int x, int y, int z) = 0;

    /*! Gets the maximum, full z level at the given x/y location. */
    virtual int getSurfaceLevel(int x, int y) = 0;

    /*! Fills a given vector with all surfaces at the given x/y location. */
    virtual void getAllSurfaces(int x, int y, std::vector <int> &surfaces) = 0;

    /*! Saves the TileGrid to the given IOTarget. */
    virtual void save(IOTarget *target) = 0;

    /*! Loads the TileGrid from the given IOTarget. */
    virtual void load(IOTarget *target) = 0;

    /*! Clears out the entire grid. */
    virtual void clear() = 0;

protected:
    int _width, _height, _depth;

};

#endif
