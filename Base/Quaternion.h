/***************************************************************************
 *                Quaternion.h
 *
 *  Thu May 25 12:59:01 2006
 *  Copyright  2005  Brent Wilson
 *  Email    llazarus@gmail.com
 *
 *  File: $Id$
 *
 *  Revisions:
 *        $Log$
 ****************************************************************************/

#ifndef _QUATERNION_H_
#define _QUATERNION_H_
#include "Vector.h"

class Matrix;
class Quaternion {
public:
    union {
        struct { Real w, x, y, z; };
        Real val[4];
    };

//C'tors
    Quaternion();
    Quaternion(const Matrix &matrix);
    Quaternion(const Quaternion &quat);
    Quaternion(Radian angle, const Vector3 &axis);
    Quaternion(const Vector3 &from, const Vector3 &to);
    Quaternion(const Vector3 &xAxis, const Vector3 &yAxis, const Vector3 &zAxis);
    Quaternion(const Radian &x, const Radian &y, const Radian &z);
    Quaternion(const Real &w, const Real &x, const Real &y, const Real &z);
    ~Quaternion();

//Axes Conversion
    void fromAxes(const Vector3 &xAxis, const Vector3 &yAxis, const Vector3 &zAxis);
    void toAxes(Vector3 &xAxis, Vector3 &yAxis, Vector3 &zAxis);

//Matrix Conversion
    Matrix toMatrix();
    void toMatrix(Matrix &matrix);
    void fromMatrix(const Matrix &mat);
    
//Euler Conversion
    void toEuler(Radian &x, Radian &y, Radian &z);
    void fromEuler(const Radian &x, const Radian &y, const Radian &z);
    
//Axis Angle Conversion
    void toAngleAxis(Radian &angle, Vector3 &axis);
    void toAngleAxis(Radian &angle, Real &x, Real &y, Real &z);
    void fromAngleAxis(const Radian &angle, const Vector3 &axis);
    void fromAngleAxis(const Radian &angle, const Real &x, const Real &y, const Real &z);

//Functions
    void invert();
    Quaternion getInverse() const;
    void apply(Vector3 &vector) const;

    void lerp(const Quaternion &other, Real percent);
    void slerp(const Quaternion &other, Real percent);

    Quaternion getLerp(const Quaternion &other, Real percent) const;
    Quaternion getSlerp(const Quaternion &other, Real percent) const;

//Operators
    Vector3 operator*(const Vector3 &rhs) const;
    Quaternion operator*(const Quaternion &rhs) const;
    Quaternion& operator*=(const Quaternion &rhs);

    Quaternion& operator=(const Quaternion &rhs);
    bool operator==(const Quaternion &rhs) const;

    Real& operator[](int index);
    const Real& operator[](int index) const;

protected:
    void normalize();
    Quaternion getNormalized() const;

    Real length() const;
    Real lengthSquared() const;

    friend class TestQuaternion;

};

std::ostream& operator<<(std::ostream &lhs, const Quaternion &rhs);

#endif    //_QUATERNION_H_
