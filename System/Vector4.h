/*
 *  Vector4.h
 *  SceneGraph
 *
 *  Created by Brent Wilson on 11/8/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _VECTOR4_H_
#define _VECTOR4_H_
#ifndef _VECTOR_H_
#error Please include Vector.h rather than Vector4.h
#endif
#include "VectorBase.h"

template<>
class Vector<4> : public VectorBase<Vector<4>, 4> {
public:
    union {
        Real array[4];
        struct { Real x, y, z, w; };
        struct { Real s, t, u, v; };
        struct { Real i, j, k, l; };
        struct { Real r, g, b, a; };
    };
    
    Vector(): VectorBase<Vector<4>, 4>() {}
    Vector(const Real *v): VectorBase<Vector<4>, 4>(v) {}
    Vector(const Vector<4> &v): VectorBase<Vector<4>, 4>(v) {}
    Vector(const Real &a, const Real &b, const Real &c, const Real &d): x(a), y(b), z(c), w(d) {}
    
    Real& operator[](int index) { return array[index]; }
    const Real& operator[](int index) const { return array[index]; }
};

typedef Vector<4> Vector4;
typedef Vector<4> Color4;
typedef Vector<4> RGBAf;

#endif
