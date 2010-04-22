#include "OctreeLiquidManager.h"

OctreeLiquidManager::OctreeLiquidManager(MHTerrain *terrain): MHLiquidManager(terrain) {
    int width = terrain->getWidth(),
        height = terrain->getHeight(),
        depth = terrain->getDepth();
    _rootPool = new LiquidPool(Vector3(0, 0, 0), Vector3(width, height, depth), Vector2(0,0.0), 0);
}

OctreeLiquidManager::~OctreeLiquidManager() {
    if(_rootPool) {
        delete _rootPool;
        _rootPool = NULL;
    }
}

// Returns the type of liquid present in [x,y,z]
short OctreeLiquidManager::getLiquidType(int x, int y, int z) {
    return _rootPool->getTile(Vector3(x,y,z))[0];
}

// Returns the amount of liquid contained in [x,y,z]
float OctreeLiquidManager::getLiquidVolume(int x, int y, int z) {
    return _rootPool->getTile(Vector3(x,y,z))[1];
}

// Sets all variables for a given [x,y,z]
void OctreeLiquidManager::setLiquid(int x, int y, int z, short type, float depth) {
    _rootPool->setTile(Vector3(x,y,z), Vector2(type, depth));
}

// Sets the type of [x,y,z]
void OctreeLiquidManager::setLiquidType(int x, int y, int z, short type) {
    _rootPool->setTile(Vector3(x,y,z), Vector2(type, 1.0));
}

// Sets the depth of [x,y,z]
void OctreeLiquidManager::setLiquidDepth(int x, int y, int z, float depth) {
    short type = _rootPool->getTile(Vector3(x,y,z))[0];
    setLiquid(x,y,z,type,depth);
}

// Adds some amount of liquid to a pool, returning the amount left over if the
// destination pool is filled in the process
float OctreeLiquidManager::addDepth(int x, int y, int z, short type, float amount) {
    Vector2 cPool = _rootPool->getTile(Vector3(x,y,z));

    if(cPool[0] != type) {
        Info("Mixing liquids not yet supported");
        return amount;
    }

    float sum = cPool[1] + amount,
          remainder = 1.0 - sum;

    if(remainder >= 0.0) {
        setLiquid(x,y,z,type,1.0);
        return remainder;
    }
    else {
        setLiquid(x,y,z,type,sum);
        return 0.0;
    }
}

// Attempt to fill to the level specified, returning the difference
float OctreeLiquidManager::fillTo(int x, int y, int z, short type, float depth) {
    Vector2 cPool = _rootPool->getTile(Vector3(x,y,z));

    if(cPool[0] != type) {
        Info("Mixing liquids not yet supported");
        return 0.0;
    }

    float difference = cPool[1] - depth;

    setLiquid(x,y,z,type,depth);

    return difference;
}
