/*
 *  Radian.cpp
 *  System
 *
 *  Created by loch on 1/30/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "Math3D.h"
#include "Radian.h"
#include "Degree.h"

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Initialization and destruction
//////////////////////////////////////////////////////////////////////////////////////////
Radian::Radian(Real r): _rad(r) {}
Radian::Radian(const Degree& d): _rad(d.valueRadians()) {}
Radian& Radian::operator=(const Real& f) { _rad = f; return *this; }
Radian& Radian::operator=(const Radian& r) { _rad = r._rad; return *this; }
Radian& Radian::operator=(const Degree& d) { _rad = d.valueRadians(); return *this; }

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Accessors
//////////////////////////////////////////////////////////////////////////////////////////
Real Radian::valueDegrees() const { return Math::Degrees(_rad); }
Real Radian::valueRadians() const { return _rad; }

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Operators
//////////////////////////////////////////////////////////////////////////////////////////
const Radian& Radian::operator+() const { return *this; }
Radian Radian::operator+(const Radian& r) const { return Radian(_rad + r._rad); }
Radian Radian::operator+(const Degree& d) const { return Radian(_rad + d.valueRadians()); }
Radian& Radian::operator+=(const Radian& r) { _rad += r._rad; return *this; }
Radian& Radian::operator+=(const Degree& d) { _rad += d.valueRadians(); return *this; }
Radian Radian::operator-() const { return Radian(-_rad); }
Radian Radian::operator-(const Radian& r) const { return Radian ( _rad - r._rad ); }
Radian Radian::operator-(const Degree& d) const { return Radian ( _rad - d.valueRadians() ); }
Radian& Radian::operator-=(const Radian& r) { _rad -= r._rad; return *this; }
Radian& Radian::operator-=(const Degree& d) { _rad -= d.valueRadians(); return *this; }
Radian Radian::operator*(Real f) const { return Radian ( _rad * f ); }
Radian Radian::operator*(const Radian& f) const { return Radian ( _rad * f._rad ); }
Radian& Radian::operator*=(Real f) { _rad *= f; return *this; }
Radian Radian::operator/(Real f) const { return Radian ( _rad / f ); }
Radian& Radian::operator/=(Real f) { _rad /= f; return *this; }

bool Radian::operator< (const Radian& r) const { return Math::lt(_rad, r._rad); }
bool Radian::operator<=(const Radian& r) const { return Math::le(_rad, r._rad); }
bool Radian::operator==(const Radian& r) const { return Math::eq(_rad, r._rad); }
bool Radian::operator!=(const Radian& r) const { return Math::ne(_rad, r._rad); }
bool Radian::operator>=(const Radian& r) const { return Math::ge(_rad, r._rad); }
bool Radian::operator> (const Radian& r) const { return Math::gt(_rad, r._rad); }

std::ostream& operator<<(std::ostream &lhs, const Radian &rhs) {
    return lhs << "Radian( " << rhs.valueRadians() << " )";
}
