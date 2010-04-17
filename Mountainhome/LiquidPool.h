#ifndef _LIQUIDPOOL_H_
#define _LIQUIDPOOL_H_

#include <Base/Vector.h>

class LiquidPool {
typedef short LiquidType;

public:
    /*! Creates a new LiquidPool.
     * \param pos    Represents the location of the lowest corner contained by this pool.
     * \param dims   The dimensions of the pool.
     * \param type   The type of liquid this pool is comprised of.
     * \param depth  The amount of liquid in this pool.
     * \param parent This pool's parent pool. */
    LiquidPool(const Vector3 &position, const Vector3 &dimensions, LiquidType type, float depth, LiquidPool *parent);

    /*! LiquidPool's destructor */
    virtual ~LiquidPool();

private:
    Vector3 _dims, _pos;
};

#endif
