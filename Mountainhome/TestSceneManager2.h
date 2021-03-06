/*
 *  TestSceneManager2.h
 *  Mountainhome
 *
 *  Created by loch on 3/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _TESTSCENEMANAGER2_H_
#define _TESTSCENEMANAGER2_H_
#include "MHSceneManager.h"

class MHWorld;

/*!\note Draws only the top level of voxels.
 * \note The top level is continuous.
 * \note Each voxel shares vertices and is drawn as a number of triangles.
 * \note When choosing the corner height, the lowest voxel at the corner is used.
 * \note Shading is smooth. */
class TestSceneManager2 : public MHSceneManager {
public:
    TestSceneManager2(MHWorld *world, MaterialManager *manager);
    virtual ~TestSceneManager2();
    virtual void populate();

protected:
    int getLowestAtCorner(int x, int y);

};

#endif
