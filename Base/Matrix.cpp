#include "Matrix.h"
#include "Assertion.h"
#include "Vector.h"
#include "Quaternion.h"

Matrix Matrix::FromEuler(const Radian &pitch, const Radian &yaw, const Radian &roll) {
    Matrix m;
    m.fromEuler(pitch, yaw, roll);
    return m;
}

Matrix Matrix::FromAxisAngle(const Radian &rad, const Vector3 &axis) {
    Matrix m;
    m.fromAxisAngle(rad, axis);
    return m;
}

Matrix Matrix::FindMatrix(const Vector3 &from, const Vector3 &to) {
    Matrix m;
    m.findMatrix(from, to);
    return m;
}

Matrix Matrix::FromAxes(const Vector3 &x, const Vector3 &y, const Vector3 &z) {
    Matrix m;
    m.fromAxes(x, y, z);
    return m;
}

Matrix Matrix::Identity() {
    Matrix m;
    m.loadIdentity();
    return m;
}

Matrix Matrix::FromTranslation(const Vector3 &translation) {
    Matrix m;
    m.setTranslation(translation);
    return m;
}

Matrix Matrix::Affine(const Quaternion &quat, const Vector3 &translation) {
    Matrix m(quat);
    m.setTranslation(translation);
    return m;
}

Matrix Matrix::InverseAffine(const Quaternion &quat, const Vector3 &translation) {
    return Matrix(quat.getInverse()) * FromTranslation(-translation);
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Initialization and destruction
//////////////////////////////////////////////////////////////////////////////////////////
Matrix::Matrix() { loadIdentity(); } ///\todo Don't load the identity by default for speed reasons.

Matrix::Matrix(const Quaternion &q) {
        Real ww = 2.0 * q[0];
    Real xx = 2.0 * q[1];
    Real yy = 2.0 * q[2];
    Real zz = 2.0 * q[3];
    
    loadIdentity();    
    m_mat[0] = 1.0 - (yy * q[2]) - (zz * q[3]);
    m_mat[1] = (xx * q[2]) + (ww * q[3]);
    m_mat[2] = (xx * q[3]) - (ww * q[2]);
    
    m_mat[4] = (xx * q[2]) - (ww * q[3]);
    m_mat[5] = 1.0 - (xx * q[1]) - (zz * q[3]);
    m_mat[6] = (yy * q[3]) + (ww * q[1]);
    
    m_mat[8] = (xx * q[3]) + (ww * q[2]);
    m_mat[9] = (yy * q[3]) - (ww * q[1]);
    m_mat[10] = 1.0 - (xx * q[1]) - (yy * q[2]);
}

Matrix::Matrix(const Real *oldMatrix) { set(oldMatrix); }
Matrix::~Matrix() {}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Axes Conversions
//////////////////////////////////////////////////////////////////////////////////////////
void Matrix::fromAxes(const Vector3 &xAxis, const Vector3 &yAxis, const Vector3 &zAxis) {
    loadIdentity();

    m_mat[0] = xAxis[0];
    m_mat[1] = xAxis[1];
    m_mat[2] = xAxis[2];

    m_mat[4] = yAxis[0];
    m_mat[5] = yAxis[1];
    m_mat[6] = yAxis[2];

    m_mat[8]  = zAxis[0];
    m_mat[9]  = zAxis[1];
    m_mat[10] = zAxis[2];
}

void Matrix::toAxes(Vector3 &xAxis, Vector3 &yAxis, Vector3 &zAxis) {
    xAxis[0] = m_mat[0];
    xAxis[1] = m_mat[1];
    xAxis[2] = m_mat[2];

    yAxis[0] = m_mat[4];
    yAxis[1] = m_mat[5];
    yAxis[2] = m_mat[6];

    zAxis[0] = m_mat[8];
    zAxis[1] = m_mat[9];
    zAxis[2] = m_mat[10];
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Axis/Angle Conversions
//////////////////////////////////////////////////////////////////////////////////////////
void Matrix::toAxisAngle(Radian &rad, Vector3 &axis) const {
    toAxisAngle(rad, axis[0], axis[1], axis[2]);
}

void Matrix::toAxisAngle(Radian &rad, Real &x, Real &y, Real &z) const {
    if (Math::eq(m_mat[1], m_mat[4]) &&
        Math::eq(m_mat[2], m_mat[8]) &&
        Math::eq(m_mat[6], m_mat[9])) {
        if (Math::ze(m_mat[4] + m_mat[1]) &&
            Math::ze(m_mat[8] + m_mat[2]) &&
            Math::ze(m_mat[9] + m_mat[6]) &&
            Math::eq(m_mat[0] + m_mat[5] + m_mat[10], 3)) {
            rad = y = z = 0;
            x = 1;
            return;
        }

        rad = Math::PI;
        x = (m_mat[0] + 1.0) / 2.0;
        x = (x > 0) ? Math::Sqrt(x) : 0;

        y = (m_mat[5] + 1.0) / 2.0;
        y = (y > 0) ? Math::Sqrt(y) : 0;

        z = (m_mat[10] + 1.0) / 2.0;
        z = (z > 0) ? Math::Sqrt(z) : 0;

        if (Math::ze(x) && !Math::ze(y) && !Math::ze(z)) {
            y = Math::Abs(y);
        } else if (Math::ze(y) && !Math::ze(z)) {
            z = Math::Abs(z);
        } else if (Math::ze(z)) {
            x = Math::Abs(x);
        }

        return;
    }

    rad = Math::Acos((m_mat[0] + m_mat[5] + m_mat[10] - 1.0) / 2.0);
    Real denom = Math::Sqrt(((m_mat[6] - m_mat[9]) * (m_mat[6] - m_mat[9])) +
                            ((m_mat[8] - m_mat[2]) * (m_mat[8] - m_mat[2])) +
                            ((m_mat[1] - m_mat[4]) * (m_mat[1] - m_mat[4])));

    x = (m_mat[6] - m_mat[9]) / denom;
    y = (m_mat[8] - m_mat[2]) / denom;
    z = (m_mat[1] - m_mat[4]) / denom;
}

void Matrix::fromAxisAngle(const Radian &rad, const Vector3 &axis) {
    loadIdentity();
    setRotation(rad, axis[0], axis[1], axis[2]);
}

void Matrix::fromAxisAngle(const Radian &rad, const Real &x, const Real &y, const Real &z) {
    loadIdentity();
    setRotation(rad, x, y, z);
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Euler Conversions
//////////////////////////////////////////////////////////////////////////////////////////
void Matrix::fromEuler(const Radian &pitch, const Radian &yaw, const Radian &roll) {
    loadIdentity();
    Real sx = Math::Sin(pitch);
    Real cx = Math::Cos(pitch);
    Real sy = Math::Sin(yaw);
    Real cy = Math::Cos(yaw);
    Real sz = Math::Sin(roll);
    Real cz = Math::Cos(roll);

    m_mat[0]  = cy * cz;
    m_mat[1]  = sz * cy;
    m_mat[2]  = -sy;

    m_mat[4]  = sx * sy * cz - sz * cx;
    m_mat[5]  = cx * cz + sx * sy * sz;
    m_mat[6]  = sx * cy;

    m_mat[8]  = sy * cx * cz + sx * sz;
    m_mat[9]  = sy * sz * cx - sx * cz;
    m_mat[10] = cx * cy;
}

void Matrix::toEuler(Radian &pitch, Radian &yaw, Radian &roll) const {
    yaw = -Math::Asin(m_mat[2]);
    Real cy = Math::Cos(yaw);
    if(Math::eq(cy, 0)) { //Gimbal Lock
        pitch = 0.0;
        roll = Math::Atan2(m_mat[4], m_mat[5]);
    } else {
        Real invCy = 1.0 / cy;
        pitch = -Math::Atan2(m_mat[6] * invCy, m_mat[10] * invCy);
        roll =  Math::Atan2(m_mat[1] * invCy, m_mat[ 0] * invCy);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Vector Application
//////////////////////////////////////////////////////////////////////////////////////////
void Matrix::apply(Vector3 &vec) const {
    Real invW = 1.0f / (vec[0] * m_mat[3] + vec[1] * m_mat[7] + vec[2] * m_mat[11] + m_mat[15]);

    Real nVec[3];
    nVec[0] = (vec[0] * m_mat[0] + vec[1] * m_mat[4] + vec[2] * m_mat[8]  + m_mat[12]) * invW;
    nVec[1] = (vec[0] * m_mat[1] + vec[1] * m_mat[5] + vec[2] * m_mat[9]  + m_mat[13]) * invW;
    nVec[2] = (vec[0] * m_mat[2] + vec[1] * m_mat[6] + vec[2] * m_mat[10] + m_mat[14]) * invW;

    memcpy(&vec, &nVec, sizeof(Vector3));
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Creation helpers
//////////////////////////////////////////////////////////////////////////////////////////
void Matrix::findMatrix(const Vector3 &from, const Vector3 &to) {
    loadIdentity();

    // Make sure we even have a rotation to make and handle it if we do.
    Radian angle = from.radiansBetween(to);
    if (angle != Radian(0)) {
        Vector3 axis;
        if (angle == Radian(Math::PI)) {
            // We've made a 180 degree turn and can't magically get an axis.
            ASSERT(!"Haven't implemented this yet!");
        } else {
            from.crossProduct(to, axis);
            axis.normalize();
        }

        // Handle the rotation
        setRotation(angle, axis);
        ASSERT_EQ(((*this) * from).getNormalized(), to.getNormalized());
    }

    // Handle the translation
    if (to.length() == 0) {
        // We're translating to the origin.
        setTranslation(from * -1);
    } else {
        setTranslation(to - (to * (from.length() / to.length())));
    }

    ASSERT_EQ(((*this) * from), to);
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Matrix operations
//////////////////////////////////////////////////////////////////////////////////////////
void Matrix::scale(const Vector3 &vec) {
    scale(vec[0], vec[1], vec[1]);
}

void Matrix::scale(Real x, Real y, Real z) {
    Matrix temp;
    temp.setScale(x, y, z);
    preMultiply(temp);
}

void Matrix::translate(const Vector3 &vec) {
    translate(vec[0], vec[1], vec[2]);
}

void Matrix::translate(Real x, Real y, Real z) {
    Matrix temp;
    temp.setTranslation(x, y, z);
    preMultiply(temp);
}

void Matrix::rotate(Radian radians, const Vector3 &axis) {
    rotate(radians, axis[0], axis[1], axis[2]);
}

void Matrix::rotate(Radian radians, Real x, Real y, Real z) {
    Matrix temp;
    temp.setRotation(radians, x, y, z);
    preMultiply(temp);
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Accessors
//////////////////////////////////////////////////////////////////////////////////////////
void Matrix::set(const Real *matrix) {
    memcpy(m_mat, matrix, sizeof(Real) * Matrix4x4);
}

void Matrix::set(const Matrix &matrix) {
    memcpy(m_mat, matrix.m_mat, sizeof(Real) * Matrix4x4);
}

void Matrix::loadIdentity() {
    memset(m_mat, 0, sizeof(Real) * Matrix4x4);
    m_mat[0] = m_mat[5] = m_mat[10] = m_mat[15] = 1;
}

void Matrix::setScale(const Vector3 &vec) {
    setScale(vec[0], vec[1], vec[2]);
}

void Matrix::setScale(Real x, Real y, Real z) {
    m_mat[0] = x;
    m_mat[5] = y;
    m_mat[10] = z;
}

void Matrix::setTranslation(const Vector3 &vec) {
    setTranslation(vec[0], vec[1], vec[2]);
}

void Matrix::setTranslation(Real x, Real y, Real z) {
    m_mat[12] = x;
    m_mat[13] = y;
    m_mat[14] = z;
}

void Matrix::setRotation(Radian radians, const Vector3 &axis) {
    setRotation(radians, axis[0], axis[1], axis[2]);
}

void Matrix::setRotation(Radian radians, Real x, Real y, Real z) {
    Real len = x * x + y * y + z * z;
    if (Math::ne(len, 1)) {
        len = Math::Sqrt(len);
        x /= len;
        y /= len;
        z /= len;
    }

    Real cTheta = Math::Cos(radians);
    Real sTheta = Math::Sin(radians);
    Real invTheta = 1 - cTheta;
    
    m_mat[ 0] = x * x * invTheta + cTheta    ;
    m_mat[ 1] = x * y * invTheta + sTheta * z;
    m_mat[ 2] = x * z * invTheta - sTheta * y;
    m_mat[ 4] = x * y * invTheta - sTheta * z;
    m_mat[ 5] = y * y * invTheta + cTheta    ;
    m_mat[ 6] = y * z * invTheta + sTheta * x;
    m_mat[ 8] = x * z * invTheta + sTheta * y;
    m_mat[ 9] = y * z * invTheta - sTheta * x;
    m_mat[10] = z * z * invTheta + cTheta    ;
}

Vector3 Matrix::getTranslation() const {
    return Vector3(m_mat[12], m_mat[13], m_mat[14]);
}

Matrix Matrix::getInverse() const {
    Real m0 = m_mat[0], m4 = m_mat[4], m8  = m_mat[ 8], m12 = m_mat[12];
    Real m1 = m_mat[1], m5 = m_mat[5], m9  = m_mat[ 9], m13 = m_mat[13];
    Real m2 = m_mat[2], m6 = m_mat[6], m10 = m_mat[10], m14 = m_mat[14];
    Real m3 = m_mat[3], m7 = m_mat[7], m11 = m_mat[11], m15 = m_mat[15];

    Real v0 = m2 * m7   - m6  * m3;
    Real v1 = m2 * m11  - m10 * m3;
    Real v2 = m2 * m15  - m14 * m3;
    Real v3 = m6 * m11  - m10 * m7;
    Real v4 = m6 * m15  - m14 * m7;
    Real v5 = m10 * m15 - m14 * m11;

    Real t0 = + (v5 * m5 - v4 * m9 + v3 * m13);
    Real t1 = - (v5 * m1 - v2 * m9 + v1 * m13);
    Real t2 = + (v4 * m1 - v2 * m5 + v0 * m13);
    Real t3 = - (v3 * m1 - v1 * m5 + v0 * m9);

    Real invDet = 1 / (t0 * m0 + t1 * m4 + t2 * m8 + t3 * m12);

    Real d0 = t0 * invDet;
    Real d1 = t1 * invDet;
    Real d2 = t2 * invDet;
    Real d3 = t3 * invDet;

    Real d4 = - (v5 * m4 - v4 * m8 + v3 * m12) * invDet;
    Real d5 = + (v5 * m0 - v2 * m8 + v1 * m12) * invDet;
    Real d6 = - (v4 * m0 - v2 * m4 + v0 * m12) * invDet;
    Real d7 = + (v3 * m0 - v1 * m4 + v0 *  m8) * invDet;

    v0 = m1 * m7  - m5  * m3;
    v1 = m1 * m11 - m9  * m3;
    v2 = m1 * m15 - m13 * m3;
    v3 = m5 * m11 - m9  * m7;
    v4 = m5 * m15 - m13 * m7;
    v5 = m9 * m15 - m13 * m11;

    Real d8  = + (v5 * m4 - v4 * m8 + v3 * m12) * invDet;
    Real d9  = - (v5 * m0 - v2 * m8 + v1 * m12) * invDet;
    Real d10 = + (v4 * m0 - v2 * m4 + v0 * m12) * invDet;
    Real d11 = - (v3 * m0 - v1 * m4 + v0 *  m8) * invDet;

    v0 = m6  * m1 - m2  * m5;
    v1 = m10 * m1 - m2  * m9;
    v2 = m14 * m1 - m2  * m13;
    v3 = m10 * m5 - m6  * m9;
    v4 = m14 * m5 - m6  * m13;
    v5 = m14 * m9 - m10 * m13;

    Real d12 = - (v5 * m4 - v4 * m8 + v3 * m12) * invDet;
    Real d13 = + (v5 * m0 - v2 * m8 + v1 * m12) * invDet;
    Real d14 = - (v4 * m0 - v2 * m4 + v0 * m12) * invDet;
    Real d15 = + (v3 * m0 - v1 * m4 + v0 *  m8) * invDet;

    Real retValues[16] = {d0,  d1,  d2,  d3,
                          d4,  d5,  d6,  d7,
                          d8,  d9,  d10, d11,
                          d12, d13, d14, d15};

    return retValues;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Operators
//////////////////////////////////////////////////////////////////////////////////////////
void Matrix::multiply(const Real* m1, const Real* m2, Real newMatrix[16]) {
    newMatrix[ 0] = m1[0] * m2[ 0] + m1[4] * m2[ 1] + m1[ 8] * m2[ 2] + m1[12] * m2[ 3];
    newMatrix[ 4] = m1[0] * m2[ 4] + m1[4] * m2[ 5] + m1[ 8] * m2[ 6] + m1[12] * m2[ 7];
    newMatrix[ 8] = m1[0] * m2[ 8] + m1[4] * m2[ 9] + m1[ 8] * m2[10] + m1[12] * m2[11];
    newMatrix[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[ 8] * m2[14] + m1[12] * m2[15];
    
    newMatrix[ 1] = m1[1] * m2[ 0] + m1[5] * m2[ 1] + m1[ 9] * m2[2 ] + m1[13] * m2[ 3];
    newMatrix[ 5] = m1[1] * m2[ 4] + m1[5] * m2[ 5] + m1[ 9] * m2[6 ] + m1[13] * m2[ 7];
    newMatrix[ 9] = m1[1] * m2[ 8] + m1[5] * m2[ 9] + m1[ 9] * m2[10] + m1[13] * m2[11];
    newMatrix[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[ 9] * m2[14] + m1[13] * m2[15];

    newMatrix[ 2] = m1[2] * m2[ 0] + m1[6] * m2[ 1] + m1[10] * m2[2 ] + m1[14] * m2[ 3];
    newMatrix[ 6] = m1[2] * m2[ 4] + m1[6] * m2[ 5] + m1[10] * m2[6 ] + m1[14] * m2[ 7];
    newMatrix[10] = m1[2] * m2[ 8] + m1[6] * m2[ 9] + m1[10] * m2[10] + m1[14] * m2[11];
    newMatrix[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];

    newMatrix[ 3] = m1[3] * m2[ 0] + m1[7] * m2[ 1] + m1[11] * m2[2 ] + m1[15] * m2[ 3];
    newMatrix[ 7] = m1[3] * m2[ 4] + m1[7] * m2[ 5] + m1[11] * m2[6 ] + m1[15] * m2[ 7];
    newMatrix[11] = m1[3] * m2[ 8] + m1[7] * m2[ 9] + m1[11] * m2[10] + m1[15] * m2[11];
    newMatrix[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];
}

void Matrix::postMultiply(const Matrix &matrix) {
    Real newMatrix[16];
    multiply(m_mat, matrix.m_mat, newMatrix);
    set(newMatrix);
}

void Matrix::preMultiply(const Matrix &matrix) {
    Real newMatrix[16];
    multiply(matrix.m_mat, m_mat, newMatrix);
    set(newMatrix);
}

Matrix Matrix::operator*(const Matrix &lhs) const {
    Matrix result(m_mat);
    result.postMultiply(lhs);
    return result;
}

Vector3 Matrix::operator*(const Vector3 &lhs) const {
    Vector3 result(lhs);
    apply(result);
    return result;
}

Matrix& Matrix::operator=(const Matrix &other) {
    set(other.m_mat);
    return (*this);
}

bool Matrix::operator==(const Matrix &other) const {
    for (int i = 0; i < 16; i++) {
        if (!Math::eq(m_mat[i], other.m_mat[i])) {
            return false;
        }
    }

    return true;
}

const Real& Matrix::operator[](int index) const {
    return m_mat[index];
}

Real& Matrix::operator[](int index) {
    return m_mat[index];
}

const Real& Matrix::operator()(int row, int col) const {
    return m_mat[(col * 4) + row];
}

Real& Matrix::operator()(int row, int col) {
    return m_mat[(col * 4) + row];
}

std::ostream& operator<<(std::ostream &lhs, const Matrix &rhs) {
    lhs << "Matrix ( ";
    lhs << "Row ( " << rhs[0] << " " << rhs[4] << " " << rhs[8 ] << " " << rhs[12] << " ) ";
    lhs << "Row ( " << rhs[1] << " " << rhs[5] << " " << rhs[9 ] << " " << rhs[13] << " ) ";
    lhs << "Row ( " << rhs[2] << " " << rhs[6] << " " << rhs[10] << " " << rhs[14] << " ) ";
    lhs << "Row ( " << rhs[3] << " " << rhs[7] << " " << rhs[11] << " " << rhs[15] << " ) ";
    lhs << " )";
    return lhs;
}
