/*
 *  Camera.h
 *  HL2-BSPReader
 *
 *  Created by Brent Wilson on 4/8/06.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _FRUSTUM_H
#define _FRUSTUM_H
#include "Vector.h"
#include "Matrix.h"
#include "AABB.h"
#include "Plane.h"

/*! The frustum is the imaginary box that extends outwards from a camera and represents
 *  the bounds of what a camera actually sees. The shape of this box is determined by the
 *  type of projection specified (either ortho or perspective). This object acts kind of
 *  like a lense for the camera and specifies what will be rendered in a given scene.
 *
 *  Perspective projection is the most typical type of projection. The as the distance
 *  from the location of the camera increases the size of the frustum increases. It is
 *  this affect that makes things appear to become smaller as the move further away from
 *  us. This is the projection type that best mirrors human vision.
 *
 *  Orthographic projection treats the camera as if it is infinitely far away. As such,
 *  the boundings lines of the frustum advance in a straight line, unlike a perspective
 *  projection in which they move outwards. Because of this difference, objects do NOT
 *  appear smaller as they move away from the screen. This projection mode is often used
 *  in cad programs.
 *  \brief A frustum that is attached to a camera. Used for culling and maintains the
 *  projection matrix.
 *  \author Brent Wilson
 *  \date 4/6/07 */
class Frustum {
public:
    enum Sides {
        LEFT = 0,
        RIGHT,
        BOTTOM,
        TOP,
        NEAR,
        FAR
    };

    enum Collision {
        COMPLETE_OUT = 0,
        INTERSECT,
        COMPLETE_IN
    };

    /*! Enumerates the two projection types. */
    enum ProjectionType {
        PERSPECTIVE, /*!< Represents a perspective projection. */
        ORTHOGRAPHIC /*!< Represents a orthographic projection. */
    };

    /*! Creates a basic frustum. The default values are set for a 45 degree viewing angle
     *  and a typical 4/3 screen ratio. The near plane is set to 1 and the far plane to
     *  1000. If these values don't work, makePerspective or makeOrtho allow for
     *  adjustments. */
    Frustum();
    virtual ~Frustum();

    /*! This method checks to see if there have been any interesting changes to the
     *  frustum and, if there has been, incorporates the changes into the stored
     *  projection matrix. This method also updates the frustum planes based on the
     *  current modelview matrix;
     * \param modelview The modelview matrix used to update the frustum planes. */
    void updateFrustum(const Matrix &modelview);

    /*! In the event of a resize, the screen ratio will need to be recalculated if the
     *  projection type is perspective. If no ratio change has occured, nothing need
     *  happen.
     * \param width The new width.
     * \param height The new height. */
    virtual void resize(int width, int height);

    /*! Sets the projection type of the frustum to perspective. Any world objects not
     *  contained in the described frustum will NOT render.
     * \param width The width of the parent render target (To calculate screen ratio).
     * \param height The height of the parent render target (To calculate screen ratio).
     * \param fov The field of view for the frustum. Wider is more.
     * \param near The distance of the near plane. Should be a small number, like 1.
     * \param far The distance of the far plane. Should be something large, like 1000. */
    void makePerspective(int width, int height, Real fov, Real near, Real far);

    /*! Sets the projection type of the frustum to orthographic. Any world objects not
     *  contained in the described frustum will NOT render.
     * \param left The location of the left plane.
     * \param right The location of the right plane.
     * \param bottom The location of the bottom plane.
     * \param top The location of the top plane.
     * \param near The location of the near plane.
     * \param far The location of the far plane. */
    void makeOrtho(Real left, Real right, Real bottom,
                   Real top, Real near, Real far);

    /*! This is a simple function that merely calls makeOrtho, and passes in default near
     *  and far values. If the camera is displaying only 2 dimentions then the extra
     *  depth of near/far is not needed.
     * \param left The location of the left plane.
     * \param right The location of the right plane.
     * \param bottom The location of the bottom plane.
     * \param top The location of the top plane. */
    void makeOrtho2D(Real left, Real right, Real bottom, Real top);    

    /*! Returns the projection matrix that describes this frustum.
     * \return The projection matrix. */
    const Matrix& getProjectionMatrix();

    //Sets the frustum up based on the clipping matrix.
    void calcFrustum(const Matrix &clipping); //Note, all normals are INWARD facing!

    //Tests to see if different shapes are inside the current frustum.
    bool checkPoint(const Vector3 &point) const;
    Collision checkSphere(const Vector3 &center, float radius) const;
    Collision checkAABB(const Vector3 &mins, const Vector3 &maxs) const;
    Collision checkAABB(const AABB3 &box) const;

    friend std::ostream& operator<<(std::ostream &lhs, const Frustum &rhs);

protected:
    ProjectionType _projectionType;
    Matrix _projectionMatrix;

    Plane _frustum[6]; //left, right, bottom, top, near, far

    Real _fov, _ratio;
    Real _near, _far;
    Real _left, _right;
    Real _bottom, _top;
    bool _valid;

protected:
    void updateOrtho();
    void updatePerspective();

    //Normalize the frustum planes.
    void normalize();

    //Plane extraction functions.
    void extractLeft(const Matrix &clipping);
    void extractRight(const Matrix &clipping);
    void extractBottom(const Matrix &clipping);
    void extractTop(const Matrix &clipping);
    void extractNear(const Matrix &clipping);
    void extractFar(const Matrix &clipping);
    
};

#endif /* _FRUSTUM_H */
