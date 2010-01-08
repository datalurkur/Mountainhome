/*
 *  Camera.h
 *  HL2-BSPReader
 *
 *  Created by Brent Wilson on 5/22/06.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _CAMERA_H
#define _CAMERA_H
#include <Base/Vector.h>
#include <list>

#include "RenderSource.h"
#include "Frustum.h"

class Camera : public RenderSource {
public:
    // C'tors and D'tors
    Camera();
    virtual ~Camera();

    //Functions
    virtual void resize(int width, int height);
    void render(RenderContext *context);

    void setPosition(const Vector3 &pos);
    void lookAt(const Vector3 &pos);

    const Frustum& getFrustum() const;
    const Vector3& getPosition() const;

    //These control camera movements
    void moveForward(float dist);
    void moveBackward(float dist)       { moveForward(-dist);    }
    void moveUpward(float dist);
    void moveDownward(float dist)       { moveUpward(-dist);    }
   
    //These control strafing movement
    void strafeRight(float dist);
    void strafeLeft(float dist)         { strafeRight(-dist);    }
   
    //These control camera rotations
    void adjustPitch(float degrees);    // Rotate the camera on the X axis
    void adjustYaw(float degrees);      // Rotate the camera on the Y axis
    void adjustRoll(float degrees);     // Rotate the camera on the Z axis
    void rotateViewRelative(Real deltaX, Real deltaY);
    void rotateView(Real deltaX, Real deltaY);
    void rotateView(float degrees, const Vector3 &axis);
    void rotateView(const Matrix &m);

    friend std::ostream& operator<<(std::ostream &lhs, const Camera &rhs);

protected:
    Vector3 _up;             //!< Vector specifying up
    Vector3 _lookAt;         //!< Vector specifying direction
    Vector3 _position;       //!< Point specifying position
    Vector3 _offset;         //!< The offset of the

    Frustum _frustum;        //!< The camera's frustum representation

    void normalize();

    friend class TestCamera;

};

#endif
