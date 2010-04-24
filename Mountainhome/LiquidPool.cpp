#include "LiquidPool.h"

LiquidPool::LiquidPool(const Vector3 &position, const Vector3 &dimensions, const Vector2 &poolData, LiquidPool *parent): 
    TileGroup<Vector2>(position, dimensions, poolData, parent) {
}

LiquidPool::~LiquidPool() {
}
