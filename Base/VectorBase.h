/*
 *  VectorBase.h
 *  3DMath
 *
 *  Created by Brent Wilson on 9/24/06.
 *  Copyright 2006 Brent Wilson. All rights reserved.
 *
 */

#ifndef _VECTORBASE_H_
#define _VECTORBASE_H_
#include "Math3D.h"
#include "Assertion.h"

#ifdef SELF
#    error SELF already defined
#endif
#define SELF (*static_cast<T*>(this))
#define CONST_SELF (*static_cast<const T*>(this))

template <typename T, int N>
class VectorBase {
public:
    VectorBase();
    VectorBase(const T &v);
    explicit VectorBase(Real value);
    explicit VectorBase(const int *v);
    explicit VectorBase(const short *v);
    explicit VectorBase(const float *v);
    explicit VectorBase(const double *v);
    
    //Functions
    void clear(Real clearVal = 0);
    const Real* ptr() const;
    Real* ptr();
    
    Real length() const;
    Real lengthSquared() const;
    Real distanceTo(const T &other) const;
    Real distanceToSquared(const T &other) const;
    bool isUnitVectorBase() const;
    Real dotProduct(const T &rhs) const;
    Radian radiansBetween(const T &rhs) const;
    Degree degreesBetween(const T &rhs) const;
    bool hasSameDirectionAs(const T &rhs) const;

    //Const modifiers
    T getAbsolute() const;
    T getNormalized() const;
    T getClamped(Real lower, Real upper) const;
    T getJittered(Real maxVariance) const;
    T getJittered(const T &maxVariance) const;
    T getFacingForward(const T &rhs) const;
    T getMinimum(const T &rhs) const;
    T getMaximum(const T &rhs) const;

    T getMidpoint(const T &endValue) const;
    T getLerp(const T &endValue, Real percent) const;
    T getReflected(const T &normal, Real restitution = 1.0f) const;

    //Self modifiers
    void absolute();
    Real normalize();
    void clamp(Real lower, Real upper);
    void jitter(Real maxVariance);
    void jitter(const T &maxVariance);
    void faceForward(const T &normal);
    void makeMinimum(const T &rhs);
    void makeMaximum(const T &rhs);

    void makeMidpoint(const T &endValue);
    void lerp(const T &endValue, Real percent);
    void reflect(const T &normal, Real restitution = 1.0f);
    
    //Operators
    T operator+(const Real &rhs) const;
    T operator-(const Real &rhs) const;
    T operator*(const Real &rhs) const;
    T operator/(const Real &rhs) const;
    T operator+(const T &rhs) const;
    T operator-(const T &rhs) const;
    T operator*(const T &rhs) const;
    T operator/(const T &rhs) const;
    
    T& operator+=(const Real &rhs);
    T& operator-=(const Real &rhs);
    T& operator*=(const Real &rhs);
    T& operator/=(const Real &rhs);
    T& operator+=(const T &rhs);
    T& operator-=(const T &rhs);
    T& operator*=(const T &rhs);
    T& operator/=(const T &rhs);
    T& operator=( const T &rhs);

    bool operator> (const T &rhs) const;
    bool operator>=(const T &rhs) const;
    bool operator< (const T &rhs) const;
    bool operator<=(const T &rhs) const;
    
    T operator-() const;
    bool operator==(const T &rhs) const;
    bool operator!=(const T &rhs) const;

};

//******************************
//******* C'Tors
//******************************
template <typename T, int N> inline
VectorBase<T, N>::VectorBase() {}

template <typename T, int N> inline
VectorBase<T, N>::VectorBase(Real value) {
    for (int i = 0; i < N; i++) {
        SELF[i] = value;
    }
}

template <typename T, int N> inline
VectorBase<T, N>::VectorBase(const T &v) {
    memcpy(&SELF[0], &v[0], sizeof(Real) * N);
}

template <typename T, int N> inline
VectorBase<T, N>::VectorBase(const float *v) {
    if (is_single<Real>::value) {
        memcpy(&SELF[0], v, sizeof(Real) * N);
    } else {
        for (int i = 0; i < N; i++) {
            SELF[i] = v[i];
        }
    }
}

template <typename T, int N> inline
VectorBase<T, N>::VectorBase(const double *v) {
    if (is_double<Real>::value) {
        memcpy(&SELF[0], v, sizeof(Real) * N);
    } else {
        for (int i = 0; i < N; i++) {
            SELF[i] = v[i];
        }
    }
}

template <typename T, int N> inline
VectorBase<T, N>::VectorBase(const int *v) {
    for (int i = 0; i < N; i++) {
        SELF[i] = v[i];
    }
}

template <typename T, int N> inline
VectorBase<T, N>::VectorBase(const short *v) {
    for (int i = 0; i < N; i++) {
        SELF[i] = v[i];
    }
}

/*
template <typename T, int N>
VectorBase<T, N>::VectorBase(const Vector2 &rhs) {
    memset(&SELF[0], 0, sizeof(Real) * N);
    memcpy(&SELF[0], &rhs, sizeof(Real) * (N >= 2 ? 2 : N));
}

template <typename T, int N>
VectorBase<T, N>::VectorBase(const Vector3 &rhs) {
    memset(&SELF[0], 0, sizeof(Real) * N);
    memcpy(&SELF[0], &rhs, sizeof(Real) * (N >= 3 ? 3 : N));
}

template <typename T, int N>
VectorBase<T, N>::VectorBase(const Vector4 &rhs) {
    memcpy(&SELF[0], &rhs, sizeof(Real) * N);
}
*/

//******************************
//******* Functions
//******************************
template <typename T, int N>
void VectorBase<T, N>::clear(Real clearVal) {
    for (int i = 0; i < N; i++) {
        SELF[i] = clearVal;
    }
}

template <typename T, int N>
const Real* VectorBase<T, N>::ptr() const {
    return &CONST_SELF[0];
}

template <typename T, int N>
Real* VectorBase<T, N>::ptr() {
    return &SELF[0];
}

template <typename T, int N>
Real VectorBase<T, N>::length() const {
    return Math::Sqrt(lengthSquared());
}

template <typename T, int N>
Real VectorBase<T, N>::lengthSquared() const {
    return dotProduct(CONST_SELF);
}

template <typename T, int N>
Real VectorBase<T, N>::distanceTo(const T &other) const {
    return Math::Sqrt(distanceToSquared(other));
}

template <typename T, int N>
Real VectorBase<T, N>::distanceToSquared(const T &other) const {
    return (other - CONST_SELF).lengthSquared();
}

template <typename T, int N>
bool VectorBase<T, N>::isUnitVectorBase() const {
    return eq(lengthSquared(), 1);
}

template <typename T, int N>
Real VectorBase<T, N>::dotProduct(const T &rhs) const {
    Real result(0);
    for (int i = 0; i < N; i++) {
        result += CONST_SELF[i] * rhs[i];
    }

    return result;
}

template <typename T, int N> //TODO Speed this up!
Radian VectorBase<T, N>::radiansBetween(const T &rhs) const {
    if (Math::eq(CONST_SELF.lengthSquared(), 0) ||
        Math::eq(rhs.lengthSquared(), 0)) {
        return Radian(0);
    }

    T a = getNormalized();
    T b = rhs.getNormalized();
    Real dot = a.dotProduct(b);

    if (Math::ge(dot, 1)) { return Radian(0); }
    else if (Math::le(dot, -1)) { return Radian(Math::PI); }
    return Math::Acos(dot);
}

template <typename T, int N>
Degree VectorBase<T, N>::degreesBetween(const T &rhs) const {
    return Degree(radiansBetween(rhs));
}

template <typename T, int N>
bool VectorBase<T, N>::hasSameDirectionAs(const T &rhs) const {
    return Math::eq(radiansBetween(rhs).valueRadians(), 0);
}

//******************************
//******* Const Functions
//******************************
template <typename T, int N>
T VectorBase<T, N>::getMinimum(const T &rhs) const {
    T result;
    for (int i = 0; i < N; i++) {
        result[i] = Math::Min(CONST_SELF[i], rhs[i]);
    }

    return result;
}

template <typename T, int N>
T VectorBase<T, N>::getMaximum(const T &rhs) const {
    T result;
    for (int i = 0; i < N; i++) {
        result[i] = Math::Max(CONST_SELF[i], rhs[i]);
    }

    return result;
}

template <typename T, int N>
T VectorBase<T, N>::getAbsolute() const {
    T result;
    for (int i = 0; i < N; i++) {
        result[i] = Math::Abs(CONST_SELF[i]);
    }

    return result;
}

template <typename T, int N>
T VectorBase<T, N>::getNormalized() const {
    T result(CONST_SELF);
    Real oldLength = length();
    if (Math::eq(oldLength, 0)) { return result; }
    return result /= oldLength;
}

template <typename T, int N>
T VectorBase<T, N>::getClamped(Real lower, Real upper) const {
    T result(CONST_SELF);
    for (int i = 0; i < N; i++) {
        clamp(lower, upper, result[i]);
    }

    return result;
}

template <typename T, int N>
T VectorBase<T, N>::getFacingForward(const T &normal) const {
    T result(CONST_SELF);
    Real value = dotProduct(normal);
    if (Math::lt(value, 0)) { result *= -1; }
    return result;
}

template <typename T, int N>
T VectorBase<T, N>::getJittered(Real maxVariance) const {
    T result(CONST_SELF);
    if (!maxVariance) { return result; }
    for (int i = 0; i < N; i++) {
        result[i] += (Math::Rand() * maxVariance * 2) - maxVariance;
    }

    return result;
}

template <typename T, int N>
T VectorBase<T, N>::getJittered(const T &rhs) const {
    T result(CONST_SELF);
    for (int i = 0; i < N; i++) {
        result[i] += (Math::Rand() * rhs[i] * 2) - rhs[i];
    }

    return result;
}

template <typename T, int N>
T VectorBase<T, N>::getMidpoint(const T &rhs) const {
    return getLerp(rhs, 0.5);
}

template <typename T, int N>
T VectorBase<T, N>::getLerp(const T &rhs, Real percent) const {
    //return SELF + ((rhs - SELF) * percent);
    T result(CONST_SELF);
    result.lerp(rhs, percent);
    return result;
}

template <typename T, int N>
T VectorBase<T, N>::getReflected(const T &normal, Real restitution) const {
    T result(CONST_SELF);
    result.reflect(normal, restitution);
    return result;
}

//******************************
//******* Self Modifiers
//******************************
template <typename T, int N>
void VectorBase<T, N>::absolute() {
    for (int i = 0; i < N; i++) {
        SELF[i] = Math::Abs(SELF[i]);
    }
}

template <typename T, int N>
Real VectorBase<T, N>::normalize() {
    Real oldLength = length();
    if (Math::eq(oldLength, 0)) { return 0; }
    SELF /= oldLength;
    return oldLength;
}

template <typename T, int N>
void VectorBase<T, N>::clamp(Real lower, Real upper) {
    ASSERT(lower < upper);
    for (int i = 0; i < N; i++) {
        if (SELF[i] < lower) { SELF[i] = lower; }
        if (SELF[i] > upper) { SELF[i] = upper; }
    }
}

template <typename T, int N>
void VectorBase<T, N>::jitter(Real maxVariance) {
    if (!maxVariance) { return; }
    for (int i = 0; i < N; i++) {
        SELF[i] += (Math::Rand() * maxVariance * 2) - maxVariance;
    }
}

template <typename T, int N>
void VectorBase<T, N>::jitter(const T &rhs) {
    for (int i = 0; i < N; i++) {
        SELF[i] += (Math::Rand() * rhs[i] * 2) - rhs[i];
    }
}

template <typename T, int N>
void VectorBase<T, N>::faceForward(const T &normal) {
    Real value = dotProduct(normal);
    if (Math::lt(value, 0)) { SELF *= -1; }
}

template <typename T, int N>
void VectorBase<T, N>::makeMidpoint(const T &rhs) {
    lerp(rhs, -.5);
}

template <typename T, int N>
void VectorBase<T, N>::lerp(const T &rhs, Real percent) {
    //SELF += ((rhs - SELF) * percent);
    for (int i = 0; i < N; i++) {
        SELF[i] += ((rhs[i] - SELF[i]) * percent);
    }
}

template <typename T, int N>
void VectorBase<T, N>::reflect(const T &normal, Real restitution) {
    Real length = VectorBase<T, N>::length();
    if (Math::eq(length, 0)) { return; }
//    assert(normal.isUnitVectorBase());

    normalize();
    SELF -= normal * 2 * dotProduct(normal);
    SELF *= length * restitution;
}

template <typename T, int N>
void VectorBase<T, N>::makeMinimum(const T &rhs) {
    for (int i = 0; i < N; i++) {
        SELF[i] = Math::Min(SELF[i], rhs[i]);
    }
}

template <typename T, int N>
void VectorBase<T, N>::makeMaximum(const T &rhs) {
    for (int i = 0; i < N; i++) {
        SELF[i] = Math::Max(SELF[i], rhs[i]);
    }
}

//******************************
//******* Operators
//******************************
template <typename T, int N>
T VectorBase<T, N>::operator+(const Real &rhs) const {
    T result(CONST_SELF);
    result += rhs;
    return result;
}

template <typename T, int N>
T VectorBase<T, N>::operator-(const Real &rhs) const {
    T result(CONST_SELF);
    result -= rhs;
    return result;
}

template <typename T, int N>
T VectorBase<T, N>::operator*(const Real &rhs) const {
    T result(CONST_SELF);
    result *= rhs;
    return result;
}

template <typename T, int N>
T VectorBase<T, N>::operator/(const Real &rhs) const {
    T result(CONST_SELF);
    result /= rhs;
    return result;
}

template <typename T, int N>
T VectorBase<T, N>::operator+(const T &rhs) const {
    T result(CONST_SELF);
    result += rhs;
    return result;
}

template <typename T, int N>
T VectorBase<T, N>::operator-(const T &rhs) const {
    T result(CONST_SELF);
    result -= rhs;
    return result;
}

template <typename T, int N>
T VectorBase<T, N>::operator*(const T &rhs) const {
    T result(CONST_SELF);
    result *= rhs;
    return result;
}

template <typename T, int N>
T VectorBase<T, N>::operator/(const T &rhs) const {
    T result(CONST_SELF);
    result /= rhs;
    return result;
}

template <typename T, int N>
T& VectorBase<T, N>::operator+=(const Real &rhs) {
    for (int i = 0; i < N; i++) {
        SELF[i] += rhs;
    }    
    return SELF;
}

template <typename T, int N>
T& VectorBase<T, N>::operator-=(const Real &rhs) {
    for (int i = 0; i < N; i++) {
        SELF[i] -= rhs;
    }    
    return SELF;
}

template <typename T, int N>
T& VectorBase<T, N>::operator*=(const Real &rhs) {
    for (int i = 0; i < N; i++) {
        SELF[i] *= rhs;
    }    
    return SELF;
}

template <typename T, int N>
T& VectorBase<T, N>::operator/=(const Real &rhs) {
    for (int i = 0; i < N; i++) {
        SELF[i] /= rhs;
    }    
    return SELF;
}

template <typename T, int N>
T& VectorBase<T, N>::operator+=(const T &rhs) {
    for (int i = 0; i < N; i++) {
        SELF[i] += rhs[i];
    }    
    return SELF;
}

template <typename T, int N>
T& VectorBase<T, N>::operator-=(const T &rhs) {
    for (int i = 0; i < N; i++) {
        SELF[i] -= rhs[i];
    }    
    return SELF;
}

template <typename T, int N>
T& VectorBase<T, N>::operator*=(const T &rhs) {
    for (int i = 0; i < N; i++) {
        SELF[i] *= rhs[i];
    }    
    return SELF;
}

template <typename T, int N>
T& VectorBase<T, N>::operator/=(const T &rhs) {
    for (int i = 0; i < N; i++) {
        SELF[i] /= rhs[i];
    }    
    return SELF;
}

template <typename T, int N>
T VectorBase<T, N>::operator-() const {
    return CONST_SELF * -1;
}

template <typename T, int N>
T& VectorBase<T, N>::operator=(const T &rhs) {
    memcpy(SELF[0], rhs[0], sizeof(Real) * N);
    return SELF;
}

/* Since there isn't a strictly well-ordered way to compare Vectors geometrically (in more than 1 dimension, anyway),
    the comparators have been implemented to mimic packing, with deference given to x, then y, then z, then w, ... */
template <typename T, int N>
bool VectorBase<T, N>::operator>(const T &rhs) const {
    for (int i = 0; i < N; i++) {
        if      (Math::lt(CONST_SELF[i], rhs[i])) { return false; }
        else if (Math::gt(CONST_SELF[i], rhs[i])) { return true;  }
    }
    return false;
}

template <typename T, int N>
bool VectorBase<T, N>::operator>=(const T &rhs) const {
    for (int i = 0; i < N; i++) {
        if      (Math::lt(CONST_SELF[i], rhs[i])) { return false; }
        else if (Math::gt(CONST_SELF[i], rhs[i])) { return true;  }
    }
    return true;
}

template <typename T, int N>
bool VectorBase<T, N>::operator<(const T &rhs) const {
    for (int i = 0; i < N; i++) {
        if      (Math::gt(CONST_SELF[i], rhs[i])) { return false; }
        else if (Math::lt(CONST_SELF[i], rhs[i])) { return true;  }
    }
    return false;
}

template <typename T, int N>
bool VectorBase<T, N>::operator<=(const T &rhs) const {
    for (int i = 0; i < N; i++) {
        if      (Math::gt(CONST_SELF[i], rhs[i])) { return false; }
        else if (Math::lt(CONST_SELF[i], rhs[i])) { return true;  }
    }
    return true;
}

template <typename T, int N>
bool VectorBase<T, N>::operator==(const T &rhs) const {
    for (int i = 0; i < N; i++) {
        if (Math::ne(CONST_SELF[i], rhs[i])) { return false; }
    }
    return true;
}

template <typename T, int N>
bool VectorBase<T, N>::operator!=(const T &rhs) const {
    return  !operator==(rhs);
}

#undef SELF
#endif
