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

/*!\note Draws ALL voxels, even underground ones.
 * \note The top level is continuous
 * \note Each voxel has it's own set of vertices and is drawn as a number of triangles.
 * \note When choosing the corner height, the highest voxel at the corner is used.
 * \note Because the highest voxel is used, we have special geometry for single unit
 *  holes. This is still broken for trenches, though.
 * \note Shading is flat.
 * \note Voxels are grouped into chunks and rendered a chunk at a time to reduce draw call
 *  counts, to reduce the number of culling checks that need to be made, and to make
 *  things like paging or geo-mipmapping easier in the future. */
class TestSceneManager1 : public MHSceneManager {
public:
    TestSceneManager1(MHWorld *world, MaterialManager *manager);
    virtual ~TestSceneManager1();
    virtual void populate();

private:
    void createChunk(int x, int y, int z);
    // Real determineZDelta(MHWorld::Voxel *base, int x, int y);

private:
    int  _voxelChunkWidth;
    int  _voxelChunkHeight;
    int  _voxelChunkDepth;

};

#endif
