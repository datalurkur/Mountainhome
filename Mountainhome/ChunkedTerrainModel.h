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

class TranslationMatrix;
class ChunkedTerrainModel : public MHReducedModel {
public:
    ChunkedTerrainModel(TileGrid *grid, TileType type,
        int xChunkIndex, int yChunkIndex, int zChunkIndex);

    virtual ~ChunkedTerrainModel();

    std::string getName();

    int update(bool doPolyReduction);

private:
    static const int ChunkSize = ChunkedTerrainGroup::ChunkSize;
    void findVertexLocForTile(int x, int y, int z, int *result, TileType cardinalType);
    void buildGeometry(
        int xPos, int yPos,
        std::vector<Vector3> &vertsArray,
        std::vector<Vector2> &texCoordsArray,
        std::vector<unsigned int> &indexArray,
        TranslationMatrix *matrix);

private:
    TileGrid *_grid;
    TileType _type;

    int _xLoc, _yLoc, _zLoc;

    std::string _name;
};

#endif
