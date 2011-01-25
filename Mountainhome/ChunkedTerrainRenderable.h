/*
 *  ChunkedTerrainRenderable.h
 *  Mountainhome
 *
 *  Created by loch on 7/12/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

///\todo XXXBMW: Needs cleanup!!!!

#ifndef _CHUNKEDTERRAINRENDERABLE_H_
#define _CHUNKEDTERRAINRENDERABLE_H_
#include <Render/Renderable.h>
#include "ChunkedTerrainGroup.h"
#include "TileGrid.h"

class ChunkedTerrainRenderable : public Renderable {
public:
    ChunkedTerrainRenderable(TileGrid *grid, PaletteIndex type,
        int xChunkIndex, int yChunkIndex, int zChunkIndex);

    virtual ~ChunkedTerrainRenderable();

    std::string getName();

    virtual int update(bool doPolyReduction) = 0;

protected:
    static const int ChunkSize = ChunkedTerrain::ChunkSize;

protected:
    TileGrid *_grid;
    PaletteIndex _type;

    int _xLoc, _yLoc, _zLoc;

    std::string _name;
};

#endif
