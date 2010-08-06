/*
 *  ChunkedTerrainModel.h
 *  Mountainhome
 *
 *  Created by loch on 7/12/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

///\todo XXXBMW: Needs cleanup!!!!

#ifndef _CHUNKEDTERRAINMODEL_H_
#define _CHUNKEDTERRAINMODEL_H_
#include <Render/Model.h>

#include "ChunkedTerrainGroup.h"
#include "TileGrid.h"

class ChunkedTerrainModel : public Model {
public:
    ChunkedTerrainModel(TileGrid *grid, TileType type,
        int xChunkIndex, int yChunkIndex, int zChunkIndex);

    virtual ~ChunkedTerrainModel();

    std::string getName();

    virtual int update(bool doPolyReduction) = 0;

protected:
    static const int ChunkSize = ChunkedTerrainGroup::ChunkSize;

protected:
    TileGrid *_grid;
    TileType _type;

    int _xLoc, _yLoc, _zLoc;

    std::string _name;
};

#endif
