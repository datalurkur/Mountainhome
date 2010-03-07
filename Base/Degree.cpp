/*
 *  Degree.cpp
 *  System
 *
 *  Created by loch on 1/30/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "Math3D.h"
#include "Degree.h"
#include "Radian.h"

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Initialization and destruction
//////////////////////////////////////////////////////////////////////////////////////////
Degree::Degree(Real d) : _deg(d) {}
Degree::Degree(const Radian& r) : _deg(r.valueDegrees()) {}
Degree& Degree::operator=(const Real& f) { _deg = f; return *this; }
Degree& Degree::operator=(const Degree& d) { _deg = d._deg; return *this; }
Degree& Degree::operator=(const Radian& r) { _deg = r.valueDegrees(); return *this; }

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Accessors
//////////////////////////////////////////////////////////////////////////////////////////
Real Degree::valueDegrees() const { return _deg; }
Real Degree::valueRadians() const { return Math::Radians(_deg); }

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Operators
//////////////////////////////////////////////////////////////////////////////////////////
const Degree& Degree::operator+() const { return *this; }
Degree Degree::operator+(const Degree& d) const { return Degree ( _deg + d._deg ); }
Degree Degree::operator+(const Radian& r) const { return Degree ( _deg + r.valueDegrees() ); }
Degree& Degree::operator+=(const Degree& d) { _deg += d._deg; return *this; }
Degree& Degree::operator+=(const Radian& r) { _deg += r.valueDegrees(); return *this; }
Degree Degree::operator-() const { return Degree(-_deg); }
Degree Degree::operator-(const Degree& d) const { return Degree ( _deg - d._deg ); }
Degree Degree::operator-(const Radian& r) const { return Degree ( _deg - r.valueDegrees() ); }
Degree& Degree::operator-=(const Degree& d) { _deg -= d._deg; return *this; }
Degree& Degree::operator-=(const Radian& r) { _deg -= r.valueDegrees(); return *this; }
Degree Degree::operator*(Real f) const { return Degree ( _deg * f ); }
Degree Degree::operator*(const Degree& f) const { return Degree ( _deg * f._deg ); }
Degree& Degree::operator*=(Real f) { _deg *= f; return *this; }
Degree Degree::operator/(Real f) const { return Degree ( _deg / f ); }
Degree& Degree::operator/=(Real f) { _deg /= f; return *this; }

bool Degree::operator< (const Degree& d) const { return Math::lt(_deg, d._deg); }
bool Degree::operator<=(const Degree& d) const { return Math::le(_deg, d._deg); }
bool Degree::operator==(const Degree& d) const { return Math::eq(_deg, d._deg); }
bool Degree::operator!=(const Degree& d) const { return Math::ne(_deg, d._deg); }
bool Degree::operator>=(const Degree& d) const { return Math::ge(_deg, d._deg); }
bool Degree::operator> (const Degree& d) const { return Math::gt(_deg, d._deg); }

std::ostream& operator<<(std::ostream &lhs, const Degree &rhs) {
    return lhs << "Degree( " << rhs.valueDegrees() << " )";
}
