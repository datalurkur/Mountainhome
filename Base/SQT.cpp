/*
 *  SQT.cpp
 *  System
 *
 *  Created by loch on 2/7/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "SQT.h"

SQT::SQT(): _translation(0.0), _scale(0.0) {}
SQT::SQT(const SQT &other) { operator=(other); }
SQT::SQT(const Matrix &matrix) { fromMatrix(matrix); }

SQT::SQT(const Quaternion &rot, const Vector3 &translate):
    _orientation(rot),
    _scale(1, 1, 1),
    _translation(translate)
{}

SQT::SQT(const Quaternion &rot, const Vector3 &scale, const Vector3 &translate):
    _orientation(rot),
    _scale(scale),
    _translation(translate)
{}

SQT::~SQT() {}

//Matrix Conversion
Matrix SQT::toMatrix() {
    Matrix m;
    toMatrix(m);
    return m;
}

void SQT::toMatrix(Matrix &matrix) {
    matrix = Matrix(_orientation);
    matrix.setTranslation(_translation);
    matrix.setScale(_scale);
}
void SQT::fromMatrix(const Matrix &matrix) {
    _translation = matrix.getTranslation();
    ///\todo Implement me
}

// Accessors
const Vector3& SQT::getScale() const { return _scale; }
const Vector3& SQT::getTrasposition() const { return _translation; }
const Quaternion& SQT::getOrientation() const { return _orientation; }

void SQT::setScale(const Vector3 &scale) { _scale = scale; }
void SQT::setTranslation(const Vector3 &translation) { _translation = translation; }
void SQT::setOrientation(const Quaternion& orientation) { _orientation = orientation; }

// Functions
void SQT::invert() {
    _orientation.invert();
    _translation.x = -_translation.x;
    _translation.y = -_translation.y;
    _translation.z = -_translation.z;
    _scale.x = 1.0 / _scale.x;
    _scale.y = 1.0 / _scale.y;
    _scale.z = 1.0 / _scale.z;
}

SQT SQT::getInverse() {
    SQT sqt(*this);
    sqt.invert();
    return sqt;
}

void SQT::apply(Vector3 &vector) const {
    _orientation.apply(vector);
    vector *= _scale;
    vector += _translation;
}

void SQT::lerp(const SQT &other, Real percent) {
    _orientation.lerp(other._orientation, percent);
    _translation.lerp(other._translation, percent);
    _scale.lerp(other._scale, percent);
}

void SQT::slerp(const SQT &other, Real percent) {
    _orientation.slerp(other._orientation, percent);
    _translation.lerp(other._translation, percent);
    _scale.lerp(other._scale, percent);
}

SQT SQT::getLerp(const SQT &other, Real percent) const {
    SQT sqt(*this);
    sqt.lerp(other, percent);
    return sqt;
}

SQT SQT::getSlerp(const SQT &other, Real percent) const {
    SQT sqt(*this);
    sqt.slerp(other, percent);
    return sqt;
}

// Operators
Vector3 SQT::operator*(const Vector3 rhs) const {
    Vector3 result(rhs);
    apply(result);
    return result;
}

SQT SQT::operator*(const SQT &rhs) const {
    SQT result(*this);
    result *= rhs;
    return result;
}

SQT& SQT::operator*=(const SQT &rhs) {
    _orientation *= rhs._orientation;
    _translation += _orientation * rhs._translation;
    _scale *= rhs._scale;
    return *this;
}

SQT& SQT::operator=(const SQT &rhs) {
    _orientation = rhs._orientation;
    _translation = rhs._translation;
    _scale = rhs._scale;
    return *this;
}

bool SQT::operator==(const SQT &rhs) const {
    return _orientation == rhs._orientation &&
        _translation == rhs._translation &&
        _scale == rhs._scale;
}
