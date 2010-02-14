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

#include "Quaternion.h"
#include "Assertion.h"
#include "Matrix.h"

Quaternion::Quaternion(): w(1), x(0), y(0), z(0) {}
Quaternion::Quaternion(const Quaternion &q)
: w(q.w), x(q.x), y(q.y), z(q.z) {}
Quaternion::Quaternion(const Real &nw, const Real &nx, const Real &ny, const Real &nz)
:w(nw), x(nx), y(ny), z(nz) {}

Quaternion::Quaternion(const Matrix &matrix) { fromMatrix(matrix); }
Quaternion::Quaternion(const Vector3 &x, const Vector3 &y, const Vector3 &z) { fromAxes(x, y, z); }
Quaternion::Quaternion(const Vector3 &from, const Vector3 &to) { fromMatrix(Matrix(from, to)); }
Quaternion::Quaternion(const Radian &x, const Radian &y, const Radian &z) { fromEuler(x, y, z); }
Quaternion::Quaternion(Radian angle, const Vector3 &axis) { fromAngleAxis(angle, axis); }

Quaternion::~Quaternion() {}

/*****************
 *Axes Conversion*
 *****************/
void Quaternion::fromAxes(const Vector3 &xAxis, const Vector3 &yAxis, const Vector3 &zAxis) {
    fromMatrix(Matrix(xAxis, yAxis, zAxis));
}

void Quaternion::toAxes(Vector3 &xAxis, Vector3 &yAxis, Vector3 &zAxis) {
    Matrix(*this).toAxes(xAxis, yAxis, zAxis);
}

/*******************
 *Matrix Conversion*
 *******************/
void Quaternion::fromMatrix(const Matrix &m) {
    Real trace = m[0] + m[5] + m[10] + 1.0;
    Real s = .5;
    
    if (!Math::eq(trace, 0)) {
        s /= Math::Sqrt(trace);
        w = 0.25 / s;
        x = (m[6] - m[9]) * s;
        y = (m[8] - m[2]) * s;
        z = (m[1] - m[4]) * s;
    } else {
        if((m[0] > m[5]) && (m[0] > m[10]) ) {
            s /= Math::Sqrt( 1.0 + m[0] - m[5] - m[10] );
            x = 0.25 / s;
            y = (m[4] + m[1]) * s;
            z = (m[8] + m[2]) * s;
            w = (m[9] - m[6]) * s;
        } else if(m[5] > m[10]) {
            s /= Math::Sqrt( 1.0 + m[5] - m[0] - m[10] );
            x = (m[4] + m[1]) * s;
            y = 0.25 / s;
            z = (m[9] + m[6]) * s;
            w = (m[8] - m[2]) * s;
        } else {
            s /= Math::Sqrt( 1.0 + m[10] - m[0] - m[5] );
            x = (m[8] + m[2]) * s;
            y = (m[9] + m[6]) * s;
            z = 0.25 / s;
            w = (m[4] - m[1]) * s;
        }
    }

    // The resulting quaternion should be already normalized.
    ASSERT_EQ(*this, getNormalized());
}

Matrix Quaternion::toMatrix() {
    Matrix m;
    toMatrix(m);
    return m;
}

void Quaternion::toMatrix(Matrix &m) {
    m.fromQuaternion(*this);
}

/******************
 *Euler Conversion*
 ******************/
void Quaternion::toEuler(Radian &nx, Radian &ny, Radian &nz) {
    // make sure we only try this with normalized quaternions
    ASSERT_EQ(*this, getNormalized());

    ny = -Math::Asin(2.0 * ((x * z) - (w * y)));
    Real cy = Math::Cos(ny);

    if(Math::eq(cy, 0)) { //Gimbal Lock
        nx = 0.0;
        nz = Math::Atan2(((x * y) - (w * z)) * 2.0, (1.0 - (x * x) - (z * z)) * 2.0);
    } else {
        Real twoInvCy = 2.0 / cy;
        nx = -Math::Atan2(((y * z) + (w * x)) * twoInvCy, (0.5 - (x * x) - (y * y)) * twoInvCy);
        nz =  Math::Atan2(((x * y) + (w * z)) * twoInvCy, (0.5 - (y * y) - (z * z)) * twoInvCy);
    }
}

void Quaternion::fromEuler(const Radian &nx, const Radian &ny, const Radian &nz) {
    Real sx = Math::Sin(nx * .5);
    Real sy = Math::Sin(ny * .5);
    Real sz = Math::Sin(nz * .5);
    Real cx = Math::Cos(nx * .5);
    Real cy = Math::Cos(ny * .5);
    Real cz = Math::Cos(nz * .5);
    
    w = (cz * cy * cx) + (sz * sy * sx);
    x = (cz * cy * sx) - (sz * sy * cx);
    y = (cz * sy * cx) + (sz * cy * sx);
    z = (sz * cy * cx) - (cz * sy * sx);

    // The resulting quaternion should be already normalized.
    ASSERT_EQ(*this, getNormalized());
}

/***********************
 *Axis Angle Conversion*
 ***********************/
void Quaternion::toAngleAxis(Radian &angle, Vector3 &axis) {
    toAngleAxis(angle, axis.x, axis.y, axis.z);
}

void Quaternion::toAngleAxis(Radian &angle, Real &nx, Real &ny, Real &nz) {
    // make sure we only try this with normalized quaternions
    ASSERT_EQ(*this, getNormalized());

    angle = Math::Acos(w) * 2.0;
    Real oneOverLength = Math::Sqrt((x * x) + (y * y) + (z * z));
    nx = x * oneOverLength;
    ny = y * oneOverLength;
    nz = z * oneOverLength;

    oneOverLength = 1.0 / Math::Sqrt((nx * nx) + (ny * ny) + (nz * nz));
    nx *= oneOverLength;
    ny *= oneOverLength;
    nz *= oneOverLength;

    // The resulting axis should be normalized already.
    ASSERT_EQ(Vector3(nx, ny, nz), Vector3(nx, ny, nz).getNormalized());
}

void Quaternion::fromAngleAxis(const Radian &angle, const Real &nx, const Real &ny, const Real &nz) {
    fromAngleAxis(angle, Vector3(nx, ny, nz));
}

void Quaternion::fromAngleAxis(const Radian &angle, const Vector3 &axis) {
    // This is used to compensate for a non-unit axis.
    Real oneOverLength = 1.0f / axis.length();
    Real sinAngle = Math::Sin(angle * 0.5f);
    
    w = Math::Cos(angle * 0.5f);
    x = (axis.x * sinAngle) * oneOverLength;
    y = (axis.y * sinAngle) * oneOverLength;
    z = (axis.z * sinAngle) * oneOverLength;

    // The resulting quaternion should be already normalized.
    ASSERT_EQ(*this, getNormalized());
}

/***********
 *Functions*
 ***********/
void Quaternion::lerp(const Quaternion &other, Real percent) {
    Real weight1 = (1.0 - percent);
    w = weight1 * w + percent * other.w;
    x = weight1 * x + percent * other.x;
    y = weight1 * y + percent * other.y;
    z = weight1 * z + percent * other.z;
    normalize();
}

void Quaternion::slerp(const Quaternion &other, Real percent) {
    Radian theta = Vector4(val).radiansBetween(Vector4(other.val));

    Real sinTheta = Math::Sin(theta);
    Real weight1 = Math::Sin(theta * (1.0 - percent)) / sinTheta;
    Real weight2 = Math::Sin(theta * (percent)) / sinTheta;

    w = weight1 * w + weight2 * other.w;
    x = weight1 * x + weight2 * other.x;
    y = weight1 * y + weight2 * other.y;
    z = weight1 * z + weight2 * other.z;
    normalize();
}

Quaternion Quaternion::getLerp(const Quaternion &other, Real percent) const {
    Quaternion result(*this);
    result.lerp(other, percent);
    return result;
}

Quaternion Quaternion::getSlerp(const Quaternion &other, Real percent) const {
    Quaternion result(*this);
    result.slerp(other, percent);
    return result;
}

void Quaternion::normalize() {
    Real len = length();
    if (Math::eq(len, 0)) { return; }
    
    x /= len;
    y /= len;
    z /= len;
    w /= len;
}

Quaternion Quaternion::getNormalized() const {
    Quaternion result(*this);
    result.normalize();
    return result;
}

void Quaternion::invert() {
    // make sure we only try this with normalized quaternions. If the quat is not of unit
    // length, the method of inverting changes.
    ASSERT_EQ(*this, getNormalized());
    x = -x;
    y = -y;
    z = -z;
}

Quaternion Quaternion::getInverse() const {
    Quaternion result(*this);
    result.invert();
    return result;
}

Real Quaternion::length() const {
    return Math::Sqrt(lengthSquared());
}

Real Quaternion::lengthSquared() const {
    return x * x + y * y + z * z + w * w;
}

void Quaternion::apply(Vector3 &vector) const {
    // make sure we only try this with normalized quaternions. If the quat is not of unit
    // length, the method of inverting changes.
    ASSERT_EQ(*this, getNormalized());

    // Tansform the vector into the 4th dimension, apply the rotation, and transform back.
    Quaternion q = (*this) * Quaternion(0, vector.x, vector.y, vector.z) * Quaternion(w, -x, -y, -z);
    vector.x = q.x;
    vector.y = q.y;
    vector.z = q.z;
}

/***********
 *Operators*
 ***********/
Vector3 Quaternion::operator*(const Vector3 &rhs) const {
    Vector3 result(rhs);
    apply(result);
    return result;
}

Quaternion Quaternion::operator*(const Quaternion &rhs) const {
    return Quaternion(w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z,
                      w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
                      w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z,
                      w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x);
}

Quaternion& Quaternion::operator*=(const Quaternion &rhs) {
    return (*this) = (*this) * rhs;
}

Quaternion& Quaternion::operator=(const Quaternion &rhs) {
    w = rhs.w;
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;

    return *this;
}

bool Quaternion::operator==(const Quaternion &rhs) const {
    return Math::eq(w, rhs.w) &&
           Math::eq(x, rhs.x) &&
           Math::eq(y, rhs.y) &&
           Math::eq(z, rhs.z);
}

Real& Quaternion::operator[](int index) {
    return val[index];
}

const Real& Quaternion::operator[](int index) const {
    return val[index];
}

std::ostream& operator<<(std::ostream &lhs, const Quaternion &rhs) {
    return lhs << "Quaternion( " << rhs.w << ", " << rhs.x << ", " << rhs.y << ", " << rhs.z << " )";
}
