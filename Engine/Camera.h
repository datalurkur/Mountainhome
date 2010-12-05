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
#include <list>

#include "SceneNode.h"
#include "RenderSource.h"
#include "ViewFrustum.h"

class SceneManager;
class Camera : public RenderSource, public SceneNode {
public:
    static const std::string TypeName;

public:
    /* Creates a copy of the current viewing frustum scaled by two corner vectors, "one" for the lower-left and "two" for the upper-right */
    void createSelectionFrustum(const Vector2 &one, const Vector2 &two, Frustum &frustum);
    void createProjectionVector(const Vector2 &vec, Vector3 &start, Vector3 &dir);

    ViewFrustum* getFrustum();

    void resize(int width, int height);
    void render(RenderContext *context);

    friend std::ostream& operator<<(std::ostream &lhs, const Camera &rhs);

protected:
    friend class SceneManager;

    Camera(const std::string &name, SceneManager *parent);
    virtual ~Camera();

protected:
    SceneManager *_parent;  //!< The scene that created this camera
    ViewFrustum _frustum;       //!< The camera's frustum representation

    friend class TestCamera;

};

#endif
