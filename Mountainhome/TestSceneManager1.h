/*
 *  TestSceneManager1.h
 *  Mountainhome
 *
 *  Created by loch on 3/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _TESTSCENEMANAGER1_H_
#define _TESTSCENEMANAGER1_H_
#include "MHSceneManager.h"
#include "MHWorld.h"

/*!\note Draws ALL tiles, even underground ones.
 * \note The top level is continuous
 * \note Each tile has it's own set of vertices and is drawn as a number of triangles.
 * \note When choosing the corner height, the highest tile at the corner is used.
 * \note Because the highest tile is used, we have special geometry for single unit
 *  holes. This is still broken for trenches, though.
 * \note Shading is flat.
 * \note Tiles are grouped into chunks and rendered a chunk at a time to reduce draw call
 *  counts, to reduce the number of culling checks that need to be made, and to make
 *  things like paging or geo-mipmapping easier in the future. */
class TestSceneManager1 : public MHSceneManager {
public:
    TestSceneManager1(MHWorld *world);
    virtual ~TestSceneManager1();
    virtual void populate();

private:
    void createChunk(int x, int y, int z);
    Real determineZDelta(MHWorld::Tile *base, int x, int y);

private:
    int  _tileChunkWidth;
    int  _tileChunkHeight;
    int  _tileChunkDepth;

};

#endif
