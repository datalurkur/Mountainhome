/*
 *  Radian.h
 *  System
 *
 *  Created by loch on 1/30/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _RADIAN_H_
#define _RADIAN_H_
#include "Base.h"

class Degree;

/*! This class is dedicated to avoiding confusion regarding rotation units (degrees v.
 *  radians). By specifically enumerating a Radian and Degree class and defining functions
 *  for automatically translating between the two, we can generally avoid bugs that pop up
 *  when someone gives a degrees where radians were expected or radians where degrees were
 *  expected while not forcing anyone to do manual conversions by hand.
 * \seealso Degree
 * \brief Represents a rotation, specified in radians.
 * \author Brent Wilson */
class Radian {
public:
#pragma mark Initialization and destruction
    Radian(Real r = 0.0);
    Radian (const Degree& d);
    Radian& operator=(const Real& f);
    Radian& operator=(const Radian& r);
    Radian& operator=(const Degree& d);

#pragma mark Accessors
    /*! Converts the stored value into degrees. */
    Real valueDegrees() const;

    /*! Converts the stored value into radians. */
    Real valueRadians() const;

#pragma mark Operators
    const Radian& operator+() const;
    Radian  operator+(const Radian& r) const;
    Radian  operator+(const Degree& d) const;
    Radian& operator+=(const Radian& r);
    Radian& operator+=(const Degree& d);
    Radian  operator-() const;
    Radian  operator-(const Radian& r) const;
    Radian  operator-(const Degree& d) const;
    Radian& operator-=(const Radian& r);
    Radian& operator-=(const Degree& d);
    Radian  operator*(Real f) const;
    Radian  operator*(const Radian& f) const;
    Radian& operator*=(Real f);
    Radian  operator/(Real f) const;
    Radian& operator/=(Real f);

    bool operator<(const Radian& r) const;
    bool operator<=(const Radian& r) const;
    bool operator==(const Radian& r) const;
    bool operator!=(const Radian& r) const;
    bool operator>=(const Radian& r) const;
    bool operator>(const Radian& r) const;

private:
    Real _rad;

};

std::ostream& operator<<(std::ostream &lhs, const Radian &rhs);

#endif
