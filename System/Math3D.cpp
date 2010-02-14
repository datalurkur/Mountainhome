/*
 *  Math.cpp
 *  3DMath
 *
 *  Created by Brent Wilson on 10/21/06.
 *  Copyright 2006 Brent Wilson. All rights reserved.
 *
 */

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "Math3D.h"

#include "AABB.h"
#include "Matrix.h"
#include "Plane.h"
#include "Quaternion.h"
#include "Ray.h"
#include "Vector.h"

namespace Math {
    unsigned int randomSeed = static_cast<unsigned int>(time(0));
    bool eq(Real a, Real b, Real error) { return Abs(a - b) < error; }
    bool ne(Real a, Real b, Real error) { return !eq(a, b, error); }
    bool le(Real a, Real b, Real error) { return a < b ||  eq(a, b, error); }
    bool ge(Real a, Real b, Real error) { return a > b ||  eq(a, b, error); }
    bool lt(Real a, Real b, Real error) { return a < b && !eq(a, b, error); }
    bool gt(Real a, Real b, Real error) { return a > b && !eq(a, b, error); }
    bool ze(Real a, Real error) { return Abs(a) < error; }

    Real Rand() {
        srand(randomSeed);
        Real ret = static_cast<Real>(drand48());
        randomSeed = rand();
        return ret;
    }

    Real Rand(const Real &upper) {
        return Rand() * upper;
    }

    Real Rand(const Real &lower, const Real &upper) {
        return Rand(upper - lower) + lower;
    }

    unsigned int RandI() {
        srand(randomSeed);
        randomSeed = rand();
        return randomSeed;
    }

    unsigned int RandI(unsigned int upper) {
        return RandI() % upper;
    }

    unsigned int RandI(unsigned int lower, unsigned int upper) {
        return RandI(upper - lower) + lower;
    }

    bool Quadratic(Real a, Real b, Real c, Real &t0, Real &t1) {
        Real val = b * b - 4 * a * c;
        if (val < 0) { return false; }
        val = Sqrt(val);
        if (b < 0)    { val = -.5 * (b - val); }
        else        { val = -.5 * (b + val); }
        t0 = val / a;
        t1 = c / val;
        if (t0 > t1) { Swap(t0, t1); }
        return true;    
    }

    Real Radians(const Real &rhs) { return rhs * PI / 180.0f; }
    Real Degrees(const Real &rhs) { return rhs * 180.0f / PI; }

    Real Sin(const Radian &rhs) { return sin(rhs.valueRadians()); }
    Real Cos(const Radian &rhs) { return cos(rhs.valueRadians()); }
    Real Tan(const Radian &rhs) { return tan(rhs.valueRadians()); }
    Real Cot(const Radian &rhs) { return 1.0 / Tan(rhs); }

    Radian Acos(const Real &rhs) { return Radian(acos(rhs)); }
    Radian Asin(const Real &rhs) { return Radian(asin(rhs)); }
    Radian Atan(const Real &rhs) { return Radian(atan(rhs)); }
    Radian Atan2(const Real &one, const Real &two) { return Radian(atan2(one, two)); }

    int IAbs(int rhs) { return abs(rhs); }
    int ICeil(const Real &rhs) { return static_cast<int>(ceil(rhs)); }
    int IFloor(const Real &rhs) { return static_cast<int>(floor(rhs)); }

    Real Abs(const Real &rhs) { return fabs(rhs); }
    Real Exp(const Real &rhs) { return exp(rhs); }
    Real Log(const Real &rhs) { return log(rhs); }
    Real Ceil(const Real &rhs) { return ceil(rhs); }
    Real Floor(const Real &rhs) { return floor(rhs); }
    Real Pow(const Real &rhs, const Real &exponent) { return pow(rhs, exponent); }

    Real Square(const Real &rhs) { return rhs * rhs; }
    Real Sqrt(const Real &rhs) { return sqrt(rhs); }
};
