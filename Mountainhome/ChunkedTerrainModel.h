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
#include "ChunkedTerrainGroup.h"
#include "MHReducedModel.h"
#include "TileGrid.h"

class ChunkedTerrainModel : public MHReducedModel {
public:
    ChunkedTerrainModel(TileGrid *grid, TileType type, int x, int y, int z);
    virtual ~ChunkedTerrainModel();

    std::string getName();

    int update();

private:
    static const int ChunkSize = ChunkedTerrainGroup::ChunkSize;

private:
    TileGrid *_grid;
    TileType _type;
    int _x, _y, _z;

    std::string _name;
};

#endif
