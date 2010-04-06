#ifndef _OCTREETERRAIN_H_
#define _OCTREETERRAIN_H_

#include "MHTerrain.h"
#include "TileGroup.h"

class OctreeTerrain: public MHTerrain {
public:
    OctreeTerrain(int width, int height, int depth);
    virtual ~OctreeTerrain();
    
    virtual short getTile(int x, int y, int z);
    virtual void setTile(int x, int y, int z, short type);
    virtual int getSurfaceLevel(int x, int y);
    
    virtual int getWidth();
    virtual int getHeight();
    virtual int getDepth();
    
    virtual void populate(MHSceneManager *scene, MaterialManager *mManager);

private:
    TileGroup<short> *_rootGroup;
};


#endif
