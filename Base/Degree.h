/*
 *  Degree.h
 *  System
 *
 *  Created by loch on 1/30/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _DEGREE_H_
#define _DEGREE_H_
#include "Base.h"

/*! This class is dedicated to avoiding confusion regarding rotation units (degrees v.
 *  radians). By specifically enumerating a Radian and Degree class and defining functions
 *  for automatically translating between the two, we can generally avoid bugs that pop up
 *  when someone gives a degrees where radians were expected or radians where degrees were
 *  expected while not forcing anyone to do manual conversions by hand.
 * \seealso Radian
 * \brief Represents a rotation, specified in degrees.
 * \author Brent Wilson */
class Radian;
class Degree {
public:
#pragma mark Initialization and destruction
    explicit Degree(Real d = 0.0);
    Degree(const Radian& r);

    Degree& operator=(const Real& f);
    Degree& operator=(const Degree& d);
    Degree& operator=(const Radian& r);

#pragma mark Accessors
    /*! Converts the stored value into degrees. */
    Real valueDegrees() const;

    /*! Converts the stored value into radians. */
    Real valueRadians() const;

#pragma mark Operators
    const Degree& operator+() const;
    Degree  operator+(const Degree& d) const;
    Degree  operator+(const Radian& r) const;
    Degree& operator+=(const Degree& d);
    Degree& operator+=(const Radian& r);
    Degree  operator-() const;
    Degree  operator-(const Degree& d) const;
    Degree  operator-(const Radian& r) const;
    Degree& operator-=(const Degree& d);
    Degree& operator-=(const Radian& r);
    Degree  operator*(Real f) const;
    Degree  operator*(const Degree& f) const;
    Degree& operator*=(Real f);
    Degree  operator/(Real f) const;
    Degree& operator/=(Real f);

    bool operator<(const Degree& d) const;
    bool operator<=(const Degree& d) const;
    bool operator==(const Degree& d) const;
    bool operator!=(const Degree& d) const;
    bool operator>=(const Degree& d) const;
    bool operator>(const Degree& d) const;

private:
    Real _deg;

};

std::ostream& operator<<(std::ostream &lhs, const Degree &rhs);

#endif
