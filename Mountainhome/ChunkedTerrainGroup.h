/*
 *  ChunkedTerrainGroup.h
 *  Mountainhome
 *
 *  Created by loch on 7/12/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _CHUNKEDTERRAINGROUP_H_
#define _CHUNKEDTERRAINGROUP_H_

class TileGrid;
class MaterialManager;
class OctreeSceneManager;
class ChunkedTerrainGroup {
public:
    ChunkedTerrainGroup(TileGrid *grid, OctreeSceneManager *scene, MaterialManager *manager);
    virtual ~ChunkedTerrainGroup();

    void updateAll();
    void update(int x, int y, int z);
    void clear();

private:
    TileGrid *_grid;
    OctreeSceneManager *_sceneManager;
    MaterialManager *_materialManager;

};

#endif
