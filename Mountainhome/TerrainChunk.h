/*
 *  TerrainChunk.h
 *  Mountainhome
 *
 *  Created by loch on 7/3/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _TERRAINCHUNK_H_
#define _TERRAINCHUNK_H_

class TerrainChunk {
public:
    TerrainChunk();
    virtual ~TerrainChunk();

    void addTile(int x, int y, int z);
    void removeTile(int x, int y, int z);
    void clear();

protected:

};

#endif
