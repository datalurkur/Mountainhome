/*
 *  HashTileGrid.h
 *  Mountainhome
 *
 *  Created by loch on 7/11/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _HashTileGrid_H_
#define _HashTileGrid_H_
#include "TileGrid.h"

class HashTileGrid : public TileGrid {
public:
    /*! Creates a new HashTileGrid of the given size. */
    HashTileGrid(int width, int height, int depth);

    /*! D'tor */
    virtual ~HashTileGrid();

    /*! Sets the tile type at the specified location. */
    virtual void setTile(int x, int y, int z, TileType type);

    /*! Gets the tile type at the specified location. */
    virtual TileType getTile(int x, int y, int z);

    /*! Gets the maximum, full z level at the given x/y location. */
    virtual int getSurfaceLevel(int x, int y);

    /*! Saves the TileGrid to the given IOTarget. */
    virtual void save(IOTarget *target);

    /*! Loads the TileGrid from the given IOTarget. */
    virtual void load(IOTarget *target);

    /*! Clears out the entire grid. */
    virtual void clear();

private:
    typedef int LookupType;
    static const size_t BitsPerDim = sizeof(LookupType) * 8 / 3;

private:
    std::map<LookupType, TileType> _typeHash;
};

#endif
