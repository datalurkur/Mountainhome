/*
 *  SQT.h
 *  System
 *
 *  Created by loch on 2/7/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _SQT_H_
#define _SQT_H_
#include "Matrix.h"
#include "Quaternion.h"
#include "Vector.h"

class SQT {
public:
    SQT();
    SQT(const SQT &other);
    SQT(const Matrix &other);
    SQT(const Quaternion &rot, const Vector3 &translate);
    SQT(const Quaternion &rot, const Vector3 &scale, const Vector3 &translate);
    ~SQT();

//Matrix Conversion
    Matrix toMatrix();
    void toMatrix(Matrix &matrix);
    void fromMatrix(const Matrix &mat);

// Accessors
    const Vector3& getScale() const;
    const Vector3& getTrasposition() const;
    const Quaternion& getOrientation() const;

    void setScale(const Vector3 &scale);
    void setTranslation(const Vector3 &translation);
    void setOrientation(const Quaternion& orientation);

// Functions
    void invert();
    SQT getInverse();

    void apply(Vector3 &vector) const;

    void lerp(const SQT &other, Real percent);
    void slerp(const SQT &other, Real percent);

    SQT getLerp(const SQT &other, Real percent) const;
    SQT getSlerp(const SQT &other, Real percent) const;

// Operators
    Vector3 operator*(const Vector3 rhs) const;
    SQT operator*(const SQT &rhs) const;
    SQT& operator*=(const SQT &rhs);

    SQT& operator=(const SQT &rhs);
    bool operator==(const SQT &rhs) const;

private:
    Quaternion _orientation;
    Vector3 _scale;
    Vector3 _translation;

};

#endif
