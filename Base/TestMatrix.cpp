/*
 *  TestMatrix.cpp
 *  Base
 *
 *  Created by Brent Wilson on 10/23/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "TestMatrix.h"
#include "Quaternion.h"
#include "Matrix.h"

void TestMatrix::RunTests() {
    TestRotation();
    TestTranslation();
    TestScaling();
    TestCombination();
    TestComplex();
    TestFindMatrix();
    TestEulerConversions();
    TestAxisAngleConversions();
    TestQuaternionConversions();
}

void TestMatrix::TestCombination() {
    Matrix m, m2;
    m.setRotation(Degree(90), Vector3(1, 0, 0));
    m.setTranslation(Vector3(0, -1, -1));
    m2.setTranslation(Vector3(5, 5, 5));
    m2.setRotation(Degree(90), Vector3(0, 1, 0));
    m.postMultiply(m2);
    Vector3 test = Vector3(0, 0, -1);
    m.apply(test);

    TASSERT_EQ(test[0], 4);
    TASSERT_EQ(test[1], -6);
    TASSERT_EQ(test[2], 4);

    Matrix m5, m6, m7;
    m5.setRotation(Degree(90), Vector3(1, 0, 0));
    //3, 1, 0
    m5.setTranslation(Vector3(90, 0, 0));
    //93, 1, 0
    m6.setRotation(Degree(90), Vector3(0, 1, 0));
    //0, 1, -93
    m6.setTranslation(Vector3(0, 90, 0));
    //0, 91, -93
    m7.setRotation(Degree(90), Vector3(0, 0, 1));
    //-91, 0, -93
    m7.setTranslation(Vector3(0, 0, 90));
    //-91, 0, -3

    m5.preMultiply(m6);
    m5.preMultiply(m7);

    Vector3 vec = Vector3(3, 0, -1);
    m5.apply(vec);

    TASSERT_EQ(vec[0], -91);
    TASSERT_EQ(vec[1], 0);
    TASSERT_EQ(vec[2], -3);
}

void TestMatrix::TestRotation() {
    Matrix mat1, mat2;
    Vector3 rot1 = Vector3(1, 0, 0);
    Vector3 rot2 = Vector3(0, 0, 1);
    Vector3 rot3 = Vector3(1,0,1);
    mat1.setRotation(Degree(90), rot1);
    mat2.setRotation(Degree(90), rot2);
    Matrix mat5 = mat1;
    mat5.preMultiply(mat2);
    Matrix mat3(mat1);
    Matrix mat4(mat3);
    mat4.preMultiply(mat2);
    
    Vector3 test1 = Vector3(0, 0, -1);
    Vector3 test2 = Vector3(0, 0, -1);
    Vector3 test3 = Vector3(0, 0, -1);
    Vector3 test4 = Vector3(0, 0, -1);
    
    mat1.apply(test1);
    mat3.apply(test2);
    TASSERT_EQ(Vector3(test1), Vector3(test2));
    TASSERT_EQ(test1[0], 0);
    TASSERT_EQ(test1[1], 1);
    TASSERT_EQ(test1[2], 0);
    
    mat2.apply(test1);
    TASSERT_EQ(test1[0], -1);
    TASSERT_EQ(test1[1], 0);
    TASSERT_EQ(test1[2], 0);
    
    mat5.apply(test4);
    TASSERT_EQ(test1, test4);

    mat4.apply(test3);
    TASSERT_EQ(test1, test3);
}

void TestMatrix::TestTranslation() {
    Matrix m, m2, m3;
    m.setTranslation(10, 9, 5);
    m2.setTranslation(-20, -1, 5);

    Vector3 v(-1, 1, 2), v2, v3, v4;
    v2 = v3 = v4 = v;

    m.apply(v);
    TASSERT_EQ(v, Vector3(9, 10, 7));
    m.postMultiply(m2);
    m.apply(v2);

    TASSERT_EQ(v2, Vector3(-11, 9, 12));

    m3.translate(10, 9, 5);
    m3.apply(v3);
    TASSERT_EQ(v3, Vector3(9, 10, 7));

    m3.translate(-20, -1, 5);
    m3.apply(v4);
    TASSERT_EQ(v4, Vector3(-11, 9, 12));
}

void TestMatrix::TestScaling() {
    Matrix m;
    m.scale(2,2,2);

    Vector3 v(1,2,1);
    m.apply(v);
    TASSERT_EQ(v, Vector3(2, 4, 2));
    m.scale(.5, .5, .5);
    m.apply(v);
    TASSERT_EQ(v, Vector3(2, 4, 2));
    m.scale(.5, .5, .5);
    m.apply(v);
    TASSERT_EQ(v, Vector3(1, 2, 1));
}

void TestMatrix::TestComplex() {
    Matrix m, m2;
    m.setRotation(Math::PI, 1, 0, 0);
    m2.setRotation(Math::PI / 2, 0, 1, 0);
    m.preMultiply(m2);

    TASSERT_EQ(m[2], -1);
    TASSERT_EQ(m[8], -1);
    TASSERT_EQ(m[5], -1);
    m.setTranslation(-2.33, 532, -3.14);
    Vector3 vec = Vector3(1, -1, 1);
    m.apply(vec);
    m.applyInverse(vec);
    TASSERT_EQ(vec[0], 1);
    TASSERT_EQ(vec[1], -1);
    TASSERT_EQ(vec[2], 1);

    
    Real a[16] = {1.81066,0,0,0,0,2.414214,0,0,0,0,-1.0002,-1,0,0,-2.0001,0};
    Real b[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,33.393997,-800,1};
    Real c[16] = {1.81066,0,0,0,0,2.414214,0,0,0,0,-1.0002,-1,0,80.620247,798.159851,800};
    Matrix A(a);
    Matrix B(b);
    Matrix C(c);

    A.postMultiply(B);
    TASSERT_EQ(A, C);
}

void TestMatrix::TestFindMatrix() {
    Vector3 one(0,0,0);
    Vector3 two(0,0,-1);
    Vector3 three(0,1,0);
    Vector3 four(0,-1,-1);
    Vector3 five(-.9390,-10,.382);
    Vector3 six(0, 5, 0);
    Matrix m;

    m.findMatrix(six, two);
    TASSERT_EQ(m * six, two);

    m.findMatrix(one, one);
    TASSERT_EQ(m * one, one);

    m.findMatrix(five, five);
    TASSERT_EQ(m * five, five);

    m.findMatrix(one, two);
    TASSERT_EQ(m * one, two);
    
    one = Vector3(0,0,0);

    m.findMatrix(two, one);
    TASSERT_EQ(m * two, one);

    two = Vector3(0,0,-1);

    m.findMatrix(two, three);
    TASSERT_EQ(m * two, three);

    two = Vector3(0,0,-1);

    m.findMatrix(three, two);
    TASSERT_EQ(m * three, two);

    three = Vector3(0,-1,0);
    m.findMatrix(two, five);
    TASSERT_EQ(m * two, five);

    two = Vector3(0,0,-1);

    m.findMatrix(five, two);
    TASSERT_EQ(m * five, two);

    five = Vector3(-.9390,-10,.382);
}

void TestMatrix::TestEulerConversions() {
    Vector3 v(0, 1, 0);
    Matrix(Math::PI / 2.0f, 0, 0).apply(v);
    TASSERT_EQ(v, Vector3(0, 0, 1));
    Matrix(0, Math::PI / 2.0f, 0).apply(v);
    TASSERT_EQ(v, Vector3(1, 0, 0));

    Matrix m(Math::PI / 4.0, 1, 0, 0);
    m.rotate(Math::PI / 4.0, 0, 1, 0);
    m.rotate(Math::PI / 4.0, 0, 0, 1);
    TASSERT_EQ(m * Vector3(.5, .5, .5),
              Matrix(Math::PI / 4.0, Math::PI / 4.0, Math::PI / 4.0) *
              Vector3(.5, .5, .5));

    Radian x = Math::PI, y = 1.1, z = -.3 * Math::PI;
    Matrix m2(x, y, z);
    Radian nx, ny, nz;
    m2.toEuler(nx, ny, nz);

    x == nx;

    TASSERT_EQ(x, nx);
    TASSERT_EQ(y, ny);
    TASSERT_EQ(z, nz);
}

void TestMatrix::TestAxisAngleConversions() {
    Radian angle(Math::PI * 0.8f);
    Radian angle2(0);
    Vector3 axis(.3, .8, 1.2);
    Vector3 axis2;

    axis.normalize();
    Matrix m(angle, axis);
    m.toAxisAngle(angle2, axis2);
    TASSERT_EQ(angle, angle2);
    TASSERT_EQ(axis, axis2);
}

void TestMatrix::TestQuaternionConversions() {
    Quaternion xRot(Math::PI / 2.0f, 0, 0);
    Quaternion yRot(0, Math::PI / 2.0f, 0);
    Vector3 rhs(.5, .5, .5);

    TASSERT_EQ(xRot * rhs, Matrix(xRot) * rhs);
    TASSERT_EQ(yRot * rhs, Matrix(yRot) * rhs);
    TASSERT_EQ(xRot * yRot * rhs, Matrix(xRot) * Matrix(yRot) * rhs);

    TASSERT_EQ(Vector3(0, 0, 1), Matrix(xRot) * Vector3(0, 1, 0));
    TASSERT_EQ(Vector3(1, 0, 0), Matrix(yRot) * Vector3(0, 0, 1));
    TASSERT_EQ(Vector3(1, 0, 0), Matrix(yRot) * Matrix(xRot) * Vector3(0, 1, 0));
    TASSERT_EQ(Vector3(0, 0, 1), Matrix(xRot) * Matrix(yRot) * Vector3(0, 1, 0));

    TASSERT_EQ(Matrix(xRot).toQuaternion(), xRot);
    TASSERT_EQ(Matrix(yRot).toQuaternion(), yRot);
}

/*
    AssertUnitVector(x, y, z);
    Real cTheta = Math::Cos(radians);
    Real sTheta = Math::Sin(radians);
    Real invTheta = 1 - cTheta;
    

    m_mat[ 1] = x * y * invTheta + sTheta * z;
    m_mat[ 2] = x * z * invTheta - sTheta * y;
    m_mat[ 4] = x * y * invTheta - sTheta * z;
    m_mat[ 6] = y * z * invTheta + sTheta * x;
    m_mat[ 8] = x * z * invTheta + sTheta * y;
    m_mat[ 9] = y * z * invTheta - sTheta * x;

     m_mat[ 0] = x * x * invTheta + cTheta;
     m_mat[ 5] = y * y * invTheta + cTheta;
    m_mat[10] = z * z * invTheta + cTheta;
 
 x^2 - (x^2 - 1) * cTheta
 y^2 - (y^2 - 1) * cTheta
 z^2 - (z^2 - 1) * cTheta
 
 xx + yy + zz - cTheta * (xx + yy + zz - 3)
 
 
 y * y * invTheta +
 z * z * invTheta +
 3 * cTheta
 
 */

/*
 m_mat[1] = sqrt((mat0  - cTheta) / (1 - cTheta)) *
            sqrt((mat5  - cTheta) / (1 - cTheta)) * 
            1 - cTheta + 
            sTheta * 
            sqrt((mat10 - cTheta) / (1 - cTheta))
 */

/*
 x = sqrt((mat0  - cTheta) / (1 - cTheta))
 y = sqrt((mat5  - cTheta) / (1 - cTheta))
 z = sqrt((mat10 - cTheta) / (1 - cTheta))

 theta = Math::Acos((x^2 - mat0 ) / (x^2 + 1))
 theta = Math::Acos((y^2 - mat5 ) / (y^2 + 1))
 theta = Math::Acos((z^2 - mat10) / (z^2 + 1))
 */
