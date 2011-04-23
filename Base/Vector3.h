/*
 *  Vector<3>.h
 *  SceneGraph
 *
 *  Created by Brent Wilson on 11/8/06.
 *  Copyright 2006 Brent Wilson. All rights reserved.
 *
 */

#ifndef _VECTOR3_H_
#define _VECTOR3_H_
#ifndef _VECTOR_H_
#error Please include Vector.h rather than Vector3.h
#endif
#include "VectorBase.h"

template<>
class Vector<3> : public VectorBase<Vector<3>, 3> {
public:
    union {
        Real array[3];
        struct { Real x, y, z; };
        struct { Real s, t, u; };
        struct { Real i, j, k; };
        struct { Real r, g, b; };
    };

    inline Vector() {}
    inline Vector(Real v): VectorBase<Vector<3>, 3>(v) {}
    inline Vector(const short *v): VectorBase<Vector<3>, 3>(v) {}
    inline Vector(const int *v): VectorBase<Vector<3>, 3>(v) {}
    inline Vector(const float *v): VectorBase<Vector<3>, 3>(v) {}
    inline Vector(const double *v): VectorBase<Vector<3>, 3>(v) {}
    inline Vector(const Vector<3> &v): VectorBase<Vector<3>, 3>(v) {}
    inline Vector(const Real &a, const Real &b, const Real &c): x(a), y(b), z(c) {}

    Real& operator[](int index) { return array[index]; }
    const Real& operator[](int index) const { return array[index]; }
    Vector<3>& crossProduct(const Vector<3> &rhs, Vector<3> &result) const {
        result[0] = array[1] * rhs[2] - array[2] * rhs[1];
        result[1] = array[2] * rhs[0] - array[0] * rhs[2];
        result[2] = array[0] * rhs[1] - array[1] * rhs[0];
        return result;
    }

    Vector<3> crossProduct(const Vector<3> &rhs) const {
        Vector<3> result;
        return crossProduct(rhs, result);
    }
};

typedef Vector<3> Vector3;
typedef Vector<3> Color3;
typedef Vector<3> RGBf;

template<>
class IVector<3> {
public:
    union {
        int array[3];
        struct { int x, y, z; };
        struct { int s, t, u; };
        struct { int i, j, k; };
        struct { int r, g, b; };
    };
    
    inline IVector() {}
    inline IVector(const IVector<3> &v): x(v.x), y(v.y), z(v.z) {}
    inline IVector(int a, int b, int c): x(a), y(b), z(c) {}
    
    int& operator[](int index) { return array[index]; }
    const int& operator[](int index) const { return array[index]; }
};

typedef IVector<3> IVector3;

#endif // _VECTOR3_H_
