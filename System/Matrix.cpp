#include "Matrix.h"
#include "Assertion.h"
#include "Vector.h"
#include "Quaternion.h"

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Initialization and destruction
//////////////////////////////////////////////////////////////////////////////////////////
Matrix::Matrix() { loadIdentity(); }
Matrix::Matrix(const Radian &yaw, const Radian &pitch, const Radian &roll) { fromEuler(yaw, pitch, roll); }
Matrix::Matrix(const Radian &rad, const Vector3 &axis) { fromAxisAngle(rad, axis); }
Matrix::Matrix(const Vector3 &from, const Vector3 &to) { findMatrix(from, to); }
Matrix::Matrix(const Vector3 &x, const Vector3 &y, const Vector3 &z) { fromAxes(x, y, z); }
Matrix::Matrix(const Quaternion &q) { fromQuaternion(q); }
Matrix::Matrix(const Real *oldMatrix) { set(oldMatrix); }
Matrix::~Matrix() {}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Axes Conversions
//////////////////////////////////////////////////////////////////////////////////////////
void Matrix::fromAxes(const Vector3 &xAxis, const Vector3 &yAxis, const Vector3 &zAxis) {
    m_mat[0] = xAxis[0];
    m_mat[4] = xAxis[1];
    m_mat[8] = xAxis[2];

    m_mat[1] = yAxis[0];
    m_mat[5] = yAxis[1];
    m_mat[9] = yAxis[2];

    m_mat[2]  = zAxis[0];
    m_mat[6]  = zAxis[1];
    m_mat[10] = zAxis[2];
}

void Matrix::toAxes(Vector3 &xAxis, Vector3 &yAxis, Vector3 &zAxis) {
    xAxis[0] = m_mat[0];
    xAxis[1] = m_mat[4];
    xAxis[2] = m_mat[8];

    yAxis[0] = m_mat[1];
    yAxis[1] = m_mat[5];
    yAxis[2] = m_mat[9];

    zAxis[0] = m_mat[2];
    zAxis[1] = m_mat[6];
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
void Matrix::fromEuler(const Radian &yaw, const Radian &pitch, const Radian &roll) {
    loadIdentity();
    Real sx = Math::Sin(yaw);
    Real cx = Math::Cos(yaw);
    Real sy = Math::Sin(pitch);
    Real cy = Math::Cos(pitch);
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

void Matrix::toEuler(Radian &yaw, Radian &pitch, Radian &roll) const {
    pitch = -Math::Asin(m_mat[2]);
    Real cy = Math::Cos(pitch);
    if(Math::eq(cy, 0)) { //Gimbal Lock
        yaw = 0.0;
        roll = Math::Atan2(m_mat[4], m_mat[5]);
    } else {
        Real invCy = 1.0 / cy;
        yaw = -Math::Atan2(m_mat[6] * invCy, m_mat[10] * invCy);
        roll =  Math::Atan2(m_mat[1] * invCy, m_mat[ 0] * invCy);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Quaternion Conversions
//////////////////////////////////////////////////////////////////////////////////////////
Quaternion Matrix::toQuaternion() const {
    Quaternion q;
    toQuaternion(q);
    return q;
}

void Matrix::toQuaternion(Quaternion &q) const {
    q.fromMatrix(*this);
}

void Matrix::fromQuaternion(const Quaternion &q) {
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

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Vector Application
//////////////////////////////////////////////////////////////////////////////////////////
void Matrix::apply(Vector3 &vec) const {
    applyRotation(vec);
    applyTranslation(vec);
}

void Matrix::applyInverse(Vector3 &vec) const {
    applyInvTranslation(vec);
    applyInvRotation(vec);
}

void Matrix::applyRotation(Vector3 &vec) const {
    Real nVec[3];
    nVec[0] = vec[0] * m_mat[0] + vec[1] * m_mat[4] + vec[2] * m_mat[8];
    nVec[1] = vec[0] * m_mat[1] + vec[1] * m_mat[5] + vec[2] * m_mat[9];
    nVec[2] = vec[0] * m_mat[2] + vec[1] * m_mat[6] + vec[2] * m_mat[10];
    memcpy(&vec, &nVec, sizeof(Vector3));
}

void Matrix::applyInvRotation(Vector3 &vec) const {
    Real nVec[3];
    nVec[0] = vec[0] * m_mat[0] + vec[1] * m_mat[1] + vec[2] * m_mat[2];
    nVec[1] = vec[0] * m_mat[4] + vec[1] * m_mat[5] + vec[2] * m_mat[6];
    nVec[2] = vec[0] * m_mat[8] + vec[1] * m_mat[9] + vec[2] * m_mat[10];
    memcpy(&vec, &nVec, sizeof(Vector3));
}

void Matrix::applyTranslation(Vector3 &vec) const {
    vec[0] += m_mat[12];
    vec[1] += m_mat[13];
    vec[2] += m_mat[14];
}

void Matrix::applyInvTranslation(Vector3 &vec) const {
    vec[0] -= m_mat[12];
    vec[1] -= m_mat[13];
    vec[2] -= m_mat[14];
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Creation helpers
//////////////////////////////////////////////////////////////////////////////////////////
void Matrix::findMatrix(const Vector3 &from, const Vector3 &to) {
    loadIdentity();

    // Make sure we even have a rotation to make and handle it if we do.
    Radian angle = from.radiansBetween(to);
    if (angle != 0) {
        Vector3 axis;
        if (angle == Math::PI) {
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

Matrix Matrix::operator*(const Matrix &lhs) {
    Matrix result(m_mat);
    result.postMultiply(lhs);
    return result;
}

Vector3 Matrix::operator*(const Vector3 &lhs) {
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
    lhs << "Matrix:" << std::endl;
    lhs << rhs[0] << " " << rhs[4] << " " << rhs[8 ] << " " << rhs[12] << std::endl;
    lhs << rhs[1] << " " << rhs[5] << " " << rhs[9 ] << " " << rhs[13] << std::endl;
    lhs << rhs[2] << " " << rhs[6] << " " << rhs[10] << " " << rhs[14] << std::endl;
    lhs << rhs[3] << " " << rhs[7] << " " << rhs[11] << " " << rhs[15] << std::endl;
    return lhs;
}
