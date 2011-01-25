/*
 *  Vector2.h
 *  SceneGraph
 *
 *  Created by Brent Wilson on 11/8/06.
 *  Copyright 2006 Brent Wilson. All rights reserved.
 *
 */

#ifndef _VECTOR2_H_
#define _VECTOR2_H_
#ifndef _VECTOR_H_
#error Please include Vector.h rather than Vector2.h
#endif
#include "VectorBase.h"

template<>
class Vector<2> : public VectorBase<Vector<2>, 2> {
public:
    union {
        Real array[2];
        struct { Real x, y; };
        struct { Real s, t; };
        struct { Real i, j; };
        struct { Real r, g; };
    };
    
    inline Vector() {}
    inline Vector(Real v): VectorBase<Vector<2>, 2>(v) {}
    inline Vector(const Real *v): VectorBase<Vector<2>, 2>(v) {}
    inline Vector(const Vector<2> &v): VectorBase<Vector<2>, 2>(v) {}
    inline Vector(const Real &a, const Real &b): x(a), y(b) {}
    
    Real& operator[](int index) { return array[index]; }
    const Real& operator[](int index) const { return array[index]; }
};

typedef Vector<2> Vector2;

template<>
class IVector<2> {
public:
    union {
        int array[2];
        struct { int x, y; };
        struct { int s, t; };
        struct { int i, j; };
        struct { int r, g; };
    };
    
    inline IVector() {}
    inline IVector(const IVector<2> &v): x(v.x), y(v.y) {}
    inline IVector(int a, int b): x(a), y(b) {}
    
    int& operator[](int index) { return array[index]; }
    const int& operator[](int index) const { return array[index]; }
};

typedef IVector<2> IVector2;

#endif
