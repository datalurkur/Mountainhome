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

    float getVolume();

private:
};

#endif
