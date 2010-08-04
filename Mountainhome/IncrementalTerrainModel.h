/*
 *  IncrementalTerrainModel.h
 *  Mountainhome
 *
 *  Created by loch on 7/3/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _INCREMENTALTERRAINMODEL_H_
#define _INCREMENTALTERRAINMODEL_H_
#include "MHModel.h"

class IncrementalTerrainModel : public MHModel {
public:
    IncrementalTerrainModel();
    virtual ~IncrementalTerrainModel();

    void addTile(int x, int y, int z);
    void removeTile(int x, int y, int z);
    void clear();

protected:

};

#endif
