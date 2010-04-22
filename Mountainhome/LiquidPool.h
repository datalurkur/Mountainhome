#ifndef _LIQUIDPOOL_H_
#define _LIQUIDPOOL_H_

#include <Base/Vector.h>
#include "TileGroup.h"

class LiquidPool: public TileGroup<Vector2> {
public:
    /*! Creates a new LiquidPool. */
    LiquidPool(const Vector3 &position, const Vector3 &dimensions, const Vector2 &poolData, LiquidPool *parent);
    /*! LiquidPool's destructor */
    virtual ~LiquidPool();

private:
};

#endif
