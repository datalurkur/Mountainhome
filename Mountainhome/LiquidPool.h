#ifndef _LIQUIDPOOL_H_
#define _LIQUIDPOOL_H_

#include <Base/Vector.h>
#include "TileGroup.h"

class LiquidPool: public TileGroup<Vector2> {
public:
    /*! Creates a new LiquidPool. */
    LiquidPool(const Vector3 &dimensions);
    /*! LiquidPool's destructor */
    virtual ~LiquidPool();

private:
};

LiquidPool::LiquidPool(const Vector3 &dimensions): TileGroup<Vector2>(Vector3(0.0), dimensions, Vector2(0.0), NULL) {
}

LiquidPool::~LiquidPool() {
}

#endif
