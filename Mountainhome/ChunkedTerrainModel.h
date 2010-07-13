/*
 *  ChunkedTerrainModel.h
 *  Mountainhome
 *
 *  Created by loch on 7/12/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _CHUNKEDTERRAINMODEL_H_
#define _CHUNKEDTERRAINMODEL_H_
#include "MHReducedModel.h"
#include "TileGrid.h"

class ChunkedTerrainModel : public MHReducedModel {
public:
    ChunkedTerrainModel(TileGrid *grid, TileType type, int x, int y, int z);
    virtual ~ChunkedTerrainModel();

    std::string getName();

    bool update(int change);

private:
    TileGrid *_grid;
    TileType _type;
    int _x, _y, _z;
    int _count;

    std::string _name;
};

#endif
