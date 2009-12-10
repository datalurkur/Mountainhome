/*
 *  Math3D.h
 *  3DMath
 *
 *  Created by Brent Wilson on 10/21/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MATH3D_H_
#define _MATH3D_H_
#include "System.h"
#include <cassert>
#include <limits>

namespace Math {
    static const Real fPointError = .001;
    static const Real PI          = 3.1415926535897932;
    static const Real HALF_PI     = PI / 2.0;
    static const Real TWO_PI      = PI * 2.0;
    static const Real Infinity    = std::numeric_limits<float>::infinity();

    bool eq(Real a, Real b, Real error = fPointError); // ==
    bool ne(Real a, Real b, Real error = fPointError); // !=
    bool le(Real a, Real b, Real error = fPointError); // <=
    bool ge(Real a, Real b, Real error = fPointError); // >=
    bool lt(Real a, Real b, Real error = fPointError); // <
    bool gt(Real a, Real b, Real error = fPointError); // >
    bool ze(Real a, Real error = fPointError); // is zero

    Real Rand();
    Real Rand(const Real &upper);
    Real Rand(const Real &lower, const Real &upper);

    unsigned int RandI();
    unsigned int RandI(unsigned int upper);
    unsigned int RandI(unsigned int lower, unsigned int upper);

    bool Quadratic(Real a, Real b, Real c, Real &t0, Real &t1);

    Real Radians(const Real &rhs);
    Real Degrees(const Real &rhs);

    Real Sin(const Real &rhs);
    Real Cos(const Real &rhs);
    Real Tan(const Real &rhs);

    Real Cot(const Real &rhs);

    Real Acos(const Real &rhs);
    Real Asin(const Real &rhs);
    Real Atan(const Real &rhs);
    Real Atan2(const Real &one, const Real &two);

    int IAbs(int rhs);
    int ICeil(const Real &rhs);
    int IFloor(const Real &rhs);

    Real Abs(const Real &rhs);

    Real Exp(const Real &rhs);
    Real Log(const Real &rhs);
    Real Ceil(const Real &rhs);
    Real Floor(const Real &rhs);
    Real Pow(const Real &rhs, const Real &exponent);

    Real Square(const Real &rhs);
    Real Sqrt(const Real &rhs);

    template<typename T1, typename T2>
    inline T1 Max(const T1 &a, const T2 &b) { return (a > b) ? a : b; }

    template<typename T1, typename T2>
    inline T1 Min(const T1 &a, const T2 &b) { return (a < b) ? a : b; }

    template<typename T>
    inline void ClampLower(T bound, T &var) { if (var < bound) { var = bound; } }

    template<typename T>
    inline void ClampUpper(T bound, T &var) { if (var > bound) { var = bound; } }

    template<typename T>
    inline void Clamp(T lower, T upper, T &var) {
        clampLower(lower, var);
        clampUpper(upper, var);
    }

    template<typename T>
    inline void Swap(T &a, T &b) {
        T temp = a;
        a = b;
        b = temp;
    }

    template <typename T>
    inline void DeletePointer(T &pointer) {
        assert(pointer);
        delete pointer;
        pointer = 0;    
    }

    template <typename T>
    inline void DeleteArray(T &pointer) {
        assert(pointer);
        delete[] pointer;
        pointer = 0;    
    }
};

#endif
