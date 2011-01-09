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
    virtual void setPaletteIndex(int x, int y, int z, PaletteIndex type);
    /*! Gets the tile type at the specified location. */
    virtual PaletteIndex getPaletteIndex(int x, int y, int z);

    /*! Gets the maximum, full z level at the given x/y location. */
    virtual int getSurfaceLevel(int x, int y);

    /*! Fills a vector with non-filled z-level ranges at the given x/y location, returning the number of ranges found. */
    virtual int getEmptyRanges(int x, int y, std::vector<std::pair<int,int> > &ranges);

    /*! Fills a given vector with all surfaces at the given x/y location. */
    virtual void getAllSurfaces(int x, int y, std::vector <int> &surfaces);

    /*! Saves the TileGrid to the given IOTarget. */
    virtual void save(IOTarget *target);

    /*! Loads the TileGrid from the given IOTarget. */
    virtual void load(IOTarget *target);

    /*! Clears out the entire grid. */
    virtual void clear();

private:
    PaletteIndex *_tileMatrix;
};

#endif
