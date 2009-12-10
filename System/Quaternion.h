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
    Quaternion(const Vector3 &euler);
    Quaternion(const Matrix &matrix);
    Quaternion(const Quaternion &quat);
    Quaternion(Real angle, const Vector3 &axis);
    Quaternion(const Real &x, const Real &y, const Real &z);
    Quaternion(const Real &w, const Real &x, const Real &y, const Real &z);
    ~Quaternion();

//Matrix Conversion
    Matrix toMatrix();
    void toMatrix(Matrix &matrix);
    void fromMatrix(const Matrix &mat);
    
//Euler Conversion
    Vector3 toEuler();
    void toEuler(Vector3 &euler);
    void toEuler(Real &x, Real &y, Real &z);
    void fromEuler(const Real &x, const Real &y, const Real &z);
    void fromEuler(const Vector3 &eulerAngles);
    
//Axis Angle Conversion
    void toAngleAxis(Real &angle, Vector3 &axis);
    void toAngleAxis(Real &angle, Real &x, Real &y, Real &z);
    void fromAngleAxis(const Real &angle, const Vector3 &axis);
    void fromAngleAxis(const Real &angle, const Real &x, const Real &y, const Real &z);

//Functions
    void invert();
    void normalize();

    Quaternion getInverse() const;
    Quaternion getNormalized() const;

    Real length() const;        ///< \todo Useful?
    Real lengthSquared() const; ///< \todo Useful?
    void apply(Vector3 &vector) const;

//Operators
    Vector3 operator*(const Vector3 &rhs) const;
    Quaternion operator*(const Real &rhs) const;
    Quaternion operator*(const Quaternion &rhs) const;
    Quaternion operator+(const Quaternion &rhs) const; ///< \todo Useful?
    Quaternion operator-(const Quaternion &rhs) const; ///< \todo Useful?

    Quaternion& operator*=(const Real &rhs);
    Quaternion& operator*=(const Quaternion &rhs);
    Quaternion& operator+=(const Quaternion &rhs); ///< \todo Useful?
    Quaternion& operator-=(const Quaternion &rhs); ///< \todo Useful?

    Quaternion& operator=(const Quaternion &rhs);
    bool operator==(const Quaternion &rhs) const;

    Real& operator[](int index);
    const Real& operator[](int index) const;

};

std::ostream& operator<<(std::ostream &lhs, const Quaternion &rhs);

#endif    //_QUATERNION_H_
