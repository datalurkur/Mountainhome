/*
 *  PositionableObject.h
 *  System
 *
 *  Created by loch on 2/1/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _POSITIONALBEOBJECT_H_
#define _POSITIONALBEOBJECT_H_
#include <Base/Quaternion.h>
#include <Base/Vector.h>
#include <Base/Matrix.h>

class PositionableObject {
public:
    PositionableObject();
    virtual ~PositionableObject();

    virtual const PositionableObject* getParent() const = 0;

    void updateDerivedValues();

    Matrix getDerivedPositionalMatrix() const;
    Matrix getPositionalMatrix() const;

    Vector3 getDerivedPosition() const;
    Vector3 getPosition() const;

    Quaternion getDerivedOrientation() const;
    Quaternion getOrientation() const;

    void setOrientation(const Quaternion &orientation);
    void setPosition(Real x, Real y, Real z);
    void setPosition(const Vector3 &pos);

    void moveAbsolute(const Vector3 &difference);
    void moveAbsolute(const Vector3 &direction, Real dist);

    void moveRelative(const Vector3 &difference);
    void moveRelative(const Vector3 &direction, Real dist);

    void moveForward(Real dist);
    void moveBackward(Real dist);
    void moveUpward(Real dist);
    void moveDownward(Real dist);
    void strafeRight(Real dist);
    void strafeLeft(Real dist);

    void rotate(Degree angle, const Vector3 &axis);
    void rotate(Radian angle, const Vector3 &axis);
    void rotate(const Quaternion &rot);

    void adjustPitch(Radian angle);
    void adjustYaw(Radian angle);
    void adjustRoll(Radian angle);

protected:
    Quaternion _derivedOrientation; //!< Maintains the derived orientation of the object.
    Quaternion _orientation;        //!< Maintains the orientation of the object.

    Vector3    _derivedPosition;    //!< Maintains the derived position of the object.
    Vector3    _position;           //!< Maintains the position of the object.

};

#endif
