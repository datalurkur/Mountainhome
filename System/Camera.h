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
#include <Base/Quaternion.h>
#include <Base/Vector.h>
#include <list>

#include "PositionableObject.h"
#include "RenderSource.h"
#include "Frustum.h"

class Scene;
class Camera : public RenderSource, public PositionableObject {
public:
    // C'tors and D'tors
    Camera();
    Camera(Scene *parent);
    virtual ~Camera();

    //Functions
    const PositionableObject* getParent() const;

    void lookAt(const Vector3 &pos);
    void setDirection(const Vector3 &dir);

    const Frustum& getFrustum() const;
    Vector3 getDirection() const;
    Vector3 getUpDirection() const;

    void resize(int width, int height);
    void render(RenderContext *context);

    friend std::ostream& operator<<(std::ostream &lhs, const Camera &rhs);

protected:
    Scene*  _parent;  //!< The scene that created this camera
    Frustum _frustum; //!< The camera's frustum representation

    friend class TestCamera;

};

#endif
