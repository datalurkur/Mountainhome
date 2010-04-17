#include "OctreeLiquidManager.h"

OctreeLiquidManager::OctreeLiquidManager(MHTerrain *terrain): MHLiquidManager(terrain) {

}

OctreeLiquidManager::~OctreeLiquidManager() {

}

// Returns the type of liquid present in [x,y,z]
int OctreeLiquidManager::getLiquidType(int x, int y, int z) {

}

// Returns the amount of liquid contained in [x,y,z]
float OctreeLiquidManager::getLiquidDepth(int x, int y, int z) {

}

// Sets all variables for a given [x,y,z]
void setLiquid(int x, int y, int z, short type, float depth) {

}

// Sets the type of [x,y,z]
void setLiquidType(int x, int y, int z, short type) {

}

// Sets the depth of [x,y,z]
void setLiquidDepth(int x, int y, int z, float depth) {

}

// Adds some amount of liquid to a pool, returning the amount left over if the
// destination pool is filled in the process
float addDepth(int x, int y, int z, float amount) {

}

// Attempt to fill to the level specified, returning the difference
float fillTo(int x, int y, int z, float depth) {

}

