/*
 *  TerrainChunkNode.h
 *  Mountainhome
 *
 *  Created by loch on 1/24/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _TERRAINCHUNKNODE_H_
#define _TERRAINCHUNKNODE_H_
#include <Engine/Entity.h>
#include "ChunkedTerrain.h"

class TerrainChunkNode : public Entity {
public:
    TerrainChunkNode(TileGrid *grid);

protected:
    static const int ChunkSize = ChunkedTerrain::ChunkSize;

};

#endif
