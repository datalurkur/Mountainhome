/*
 *  TestSceneManager3.h
 *  Mountainhome
 *
 *  Created by loch on 3/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _TESTSCENEMANAGER3_H_
#define _TESTSCENEMANAGER3_H_
#include "MHSceneManager.h"

class MHWorld;

/*!\note Draws only tiles that have empty space adjacent.
 * \note Each tile has its own set of vertices and is drawn as a number of triangles.
 * \note When choosing the corner height, the lowest tile at the corner is used.
 * \note Shading is flat. */
class TestSceneManager3 : public MHSceneManager {
public:
    TestSceneManager3(MHWorld *world, MaterialManager *manager);
    virtual ~TestSceneManager3();
    virtual void populate();

protected:
    Vector3 getVectorForCorner(int x, int y) const ;
    int getLowestAtCorner(int x, int y) const;

};

#endif
