/*
 *  MatrixTileGrid.h
 *  Mountainhome
 *
 *  Created by loch on 7/9/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MATRIXTILEGRID_H_
#define _MATRIXTILEGRID_H_
#include "TileGrid.h"

class MatrixTileGrid : public TileGrid {
public:

    /*! Creates a new MatrixTileGrid of the given size. */
    MatrixTileGrid(int width, int height, int depth);

    /*! D'tor */
    virtual ~MatrixTileGrid();

    /*! Sets the tile type at the specified location. */
    virtual void setTile(int x, int y, int z, Tile type);
    /*! Sets the tile type at the specified location. */
    virtual void setTileType(int x, int y, int z, TileType type);
    virtual void setTileParameter(int x, int y, int z, TileParameter param, bool value);

    /*! Gets the tile type at the specified location. */
    virtual Tile getTile(int x, int y, int z);
    /*! Gets the tile type at the specified location. */
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
    Tile *_tileMatrix;
};

#endif
