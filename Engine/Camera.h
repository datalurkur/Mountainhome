/*
 *  Camera.h
 *  Render
 *
 *  Created by Brent Wilson on 5/22/06.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _CAMERA_H
#define _CAMERA_H
#include <Base/Quaternion.h>
#include <Base/Vector.h>
#include <Base/Frustum.h>
#include <list>

#include "SceneNode.h"

class SceneManager;
class Camera : public SceneNode {
public:
    static const std::string TypeName;

public:
    Camera(const std::string &name);
    virtual ~Camera();

    /* Creates a copy of the current viewing frustum scaled by two corner vectors, "one"
     * for the lower-left and "two" for the upper-right */
    void createSelectionFrustum(const Vector2 &one, const Vector2 &two, Frustum &frustum);

    void createProjectionVector(const Vector2 &vec, Vector3 &start, Vector3 &dir);

    void setProjectionMatrix(const Matrix &matrix);

    const Matrix & getProjectionMatrix();

    const Matrix & getViewMatrix();

    const Frustum & getFrustum();

    friend std::ostream& operator<<(std::ostream &lhs, const Camera &rhs);

protected:
    Camera(const std::string &name, const std::string &type);

    virtual bool updateImplementationValues();

protected:
    Frustum _frustum;
    Matrix _projection;
    Matrix _view;

    friend class TestCamera;
};

#endif
