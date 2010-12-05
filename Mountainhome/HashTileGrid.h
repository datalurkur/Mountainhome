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

    virtual void setTile(int x, int y, int z, Tile type);
    /*! Sets the tile bits at the specified location. */
    virtual void setTileType(int x, int y, int z, TileType type);
    virtual void setTileParameter(int x, int y, int z, TileParameter param, bool value);

    virtual Tile getTile(int x, int y, int z);
    /*! Gets the tile bits at the specified location. */
    virtual TileType getTileType(int x, int y, int z);
    virtual bool getTileParameter(int x, int y, int z, TileParameter param);

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

    // Convert the LookupType size from bytes to bits and split it into thirds (one for each dimension).
    static const size_t BitsPerDim = sizeof(LookupType) * 8 / 3;

private:
    std::map<LookupType, Tile> _tileHash;
};

#endif
