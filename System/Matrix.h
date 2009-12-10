#ifndef _MATRIX_H
#define _MATRIX_H

#include "Vector.h"

//   | m00 m10 m20 tx |    |  m0  m4  m8 m12 |    |  m0  m1  m2  m3 |
//   | m01 m11 m21 ty |    |  m1  m5  m9 m13 |    |  m4  m5  m6  m7 |
//   | m02 m12 m22 tz |    |  m2  m6 m10 m14 |    |  m8  m9 m10 m11 |
//   |  0   0   0  tw |    |  m3  m7 m11 m15 |    | m12 m13 m14 m15 |
//
//      OpenGL style          OpenGL matrix            standard C
//                             arrangement             convention
//
//This matrix class is based on the openGL implimentation of a matirx

class Quaternion;
class Matrix {
    static const int Matrix4x4 = 16;
    Real m_mat[Matrix4x4];

public:
//C'tors
    Matrix();
    Matrix(const Vector3 &euler);
    Matrix(const Real &eulerXrad, const Real &eulerYrad, const Real &eulerZrad);
    Matrix(const Real &rad, const Vector3 &axis);
    Matrix(const Real &rad, const Real &x, const Real &y, const Real &z);
    Matrix(const Vector3 &from, const Vector3 &to);
    Matrix(const Quaternion &quat);
    Matrix(const Real *oldMatrix);
    ~Matrix();

//Axis/Angle Conversions
    void toAxisAngle(Real &rad, Vector3 &axis) const;
    void toAxisAngle(Real &rad, Real &x, Real &y, Real &z) const;
    void fromAxisAngle(const Real &rad, const Vector3 &axis);
    void fromAxisAngle(const Real &rad, const Real &x, const Real &y, const Real &z);

//Euler Conversions
    Vector3 toEuler() const;
    void toEuler(Vector3 &euler) const;
    void toEuler(Real &x, Real &y, Real &z) const;

    void fromEuler(const Vector3 &euler);
    void fromEuler(const Real &x, const Real &y, const Real &z);

//Quaternion Conversions
    Quaternion toQuaternion() const;
    void toQuaternion(Quaternion &q) const;
    void fromQuaternion(const Quaternion &q);

//Vector Application
    void apply(Vector3 &vec) const;
    void applyInverse(Vector3 &vec) const;
    void applyRotation(Vector3 &vec) const;
    void applyInvRotation(Vector3 &vec) const;
    void applyTranslation(Vector3 &vec) const;
    void applyInvTranslation(Vector3 &vec) const;

//GL Helpers
    void makeClippingMatrix();
    void makeProjectionMatrix();
    void makeModelviewMatrix();
    void findMatrix(const Vector3 &from, const Vector3 &to);
    
//Matrix Operations
//These create a matrix (A) and multiply it by the current matrix (B)
//and sets the current matrix to the result: B = A * B
//This allows you to easily chain events as the current matrix will apply
//folowed by the created matrix.
    void scale(const Vector3 &vec);
    void scale(Real x, Real y, Real z);
    void translate(const Vector3 &vec);
    void translate(Real x, Real y, Real z);
    void rotate(Real radians, const Vector3 &axis);
    void rotate(Real radians, Real x, Real y, Real z);
    void rotateD(Real degrees, const Vector3 &axis);
    void rotateD(Real degrees, Real x, Real y, Real z);

//Accessors
    void loadIdentity();
    void set(const Matrix &matrix);
    void set(const Real *matrix);
    const Real *getMatrix() const { return m_mat; }

    void setScale(const Vector3 &scale);
    void setScale(Real x, Real y, Real z);
    void setTranslation(const Vector3 &offset);
    void setTranslation(Real x, Real y, Real z);
    void setRotation(Real radians, const Vector3 &scale);
    void setRotation(Real radians, Real x, Real y, Real z);
    void setRotationD(Real degrees, const Vector3 &scale);
    void setRotationD(Real degrees, Real x, Real y, Real z);
    
//Operators
    void multiply(const Real* m1, const Real* m2, Real newMatrix[16]);
    void postMultiply(const Matrix &rhs);
    void preMultiply(const Matrix &rhs);

    Matrix operator*(const Matrix &lhs);
    Vector3 operator*(const Vector3 &lhs);

    Matrix& operator=(const Matrix &other);
    bool operator==(const Matrix &other) const;
    bool operator!=(const Matrix &other) const;

    const Real& operator[](int index) const;
    Real& operator[](int index);

    const Real& operator()(int row, int col) const;
    Real& operator()(int row, int col);
};

std::ostream& operator<<(std::ostream &lhs, const Matrix &rhs);
#endif
