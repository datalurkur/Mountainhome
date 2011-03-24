/*
 *  SceneNode.h
 *  System
 *
 *  Created by loch on 2/1/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _SceneNode_H_
#define _SceneNode_H_
#include <Base/Quaternion.h>
#include <Base/Vector.h>
#include <Base/Matrix.h>
#include <Base/AABB.h>

#include <Render/Renderable.h>

class SceneNode;
typedef std::list<SceneNode*> SceneNodeList;
typedef std::map<std::string, SceneNode*> SceneNodeMap;

class SceneManager;
class Frustum;

class SceneNode {
public:
    static const std::string TypeName;
    friend class SceneManager;

public:
    SceneNode(const std::string &name);
    virtual ~SceneNode();

    virtual void preRenderNotice() {}

    void addVisibleObjectsToList(const Frustum &bounds, SceneNodeList &visible);
    void addAllObjectsToList(SceneNodeList &objects);

    /*! Adds any renderables associated with the scene node to the given RenderableList. */
    virtual void addRenderablesToList(RenderableList &list, bool includeBB=true);

    void addRenderable(Renderable *renderable);
    void removeRenderable(Renderable *renderable);

    const std::string &getName() const;
    const std::string &getType() const;
    SceneNode* getParent() const;

    void attach(SceneNode *obj);
    void dettach(SceneNode *obj);
    void dettachAllChildren();

    const Matrix & getDerivedTransformationMatrix() const;
    const Matrix & getLocalTransformationMatrix() const;

    const AABB3 & getDerivedAABB() const;

    Vector3 getDerivedPosition() const;
    Vector3 getLocalPosition() const;

    Quaternion getDerivedOrientation() const;
    Quaternion getLocalOrientation() const;

    void setOrientation(const Quaternion &orientation);
    void setPosition(Real x, Real y, Real z);
    void setPosition(const Vector3 &pos);

    void setPositionX(Real x);
    void setPositionY(Real y);
    void setPositionZ(Real z);

    Real getPositionX();
    Real getPositionY();
    Real getPositionZ();

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

    void lookAt(const Vector3 &pos);
    void setDirection(const Vector3 &dir);
    Vector3 getDirection() const;
    Vector3 getUpDirection() const;

    /*! Sets a fixed axis for yaw adjustments. This essentially makes sure the object is
     *  always vertically aligned along this axis. */
    void setFixedYawAxis(bool fixed, const Vector3 &axis = Vector3(0, 1, 0));

    /*! Gets the current fixed yaw axis. */
    Vector3 getFixedYawAxis() const;

    void setVisibility(bool state);

protected:
    SceneNode(const std::string &name, const std::string &type);

    /*! Updates this object's derived position and orientation values and calls 
     *  updateImplementationValues to update other values. */
    virtual void updateDerivedValues();

    /*! Called after updateDerivedValues. This function updates any other internal values
     *  that need to be changed when the position and orientation change. This include,
     *  by default, the objects bounding box. */
    virtual void updateImplementationValues();

    /*! Sets the dirty bit for this object. This indicates it has been moved and needs to
     *  be updated for internal values to be valid again. This is made virtual so
     *  subclasses can individually handle being dirtied. */
    virtual void setDirty(bool value = true);

    /*! Checks the dirty bit for this object. */
    bool isDirty() const;

    void updateRenderableViewMatrices();
    void updateTransformationMatrices();

protected:
    bool       _dirty;               //!< Whether or not the position or orientation has changes and needs to be updated.
    bool       _fixedYawAxis;        //!< Whether or not the yaw axis is fixed.
    Vector3    _yawAxis;             //!< The axis to rotate about when the yaw axis is fixed.

    Quaternion _derivedOrientation;  //!< Maintains the derived orientation of the object.
    Quaternion _orientation;         //!< Maintains the orientation of the object.

    Vector3    _derivedPosition;     //!< Maintains the derived position of the object.
    Vector3    _position;            //!< Maintains the position of the object.

    Matrix _transform;
    Matrix _derivedTransform;

    AABB3 _derivedBoundingBox;       //!< Maintains the derived AABB of the object.

    SceneNodeMap _children; //!< This object's children.
    SceneNode *_parent;     //!< This object's parent.

    std::string _type; //!< The object's type name.
    std::string _name; //!< The object's name.

    bool _visible;

    RenderableList _renderables;
    Renderable *_boundingBoxRenderable;
};

#endif
