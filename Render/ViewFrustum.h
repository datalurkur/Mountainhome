/*
 *  ViewFrustum.h
 *  Mountainhome
 *
 *  Created by loch on 10/16/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _VIEWFRUSTUM_H_
#define _VIEWFRUSTUM_H_

#include <Base/Frustum.h>

class ViewFrustum : public Frustum {
public:

    /*! Enumerates the two projection types. */
    enum ProjectionType {
        PERSPECTIVE, /*!< Represents a perspective projection. */
        ORTHOGRAPHIC /*!< Represents a orthographic projection. */
    };

public:
    /*! Creates a basic frustum. The default values are set for a 45 degree viewing angle
     *  and a typical 4/3 screen ratio. The near plane is set to 1 and the far plane to
     *  1000. If these values don't work, makePerspective or makeOrtho allow for
     *  adjustments. */
    ViewFrustum();
    virtual ~ViewFrustum();

    /*! This method checks to see if there have been any interesting changes to the
     *  frustum and, if there has been, incorporates the changes into the stored
     *  projection matrix. This method also updates the frustum planes based on the
     *  current modelview matrix;
     * \param modelview The modelview matrix used to update the frustum planes. */
    void setTransformation(const Matrix &modelview);

    //Sets the frustum up based on the clipping matrix.
    void calcFrustum(const Matrix &clipping); //Note, all normals are INWARD facing!

    /*! Accessor for the screen dimension ratio */
    Real getRatio();

    /*! In the event of a resize, the screen ratio will need to be recalculated if the
     *  projection type is perspective. If no ratio change has occured, nothing need
     *  happen.
     * \param width The new width.
     * \param height The new height. */
    virtual void resize(int width, int height);

    /*! Sets the projection type of the frustum to perspective. Any world objects not
     *  contained in the described frustum will NOT render.
     * \param fov The field of view for the frustum. Wider means more is visible.
     * \param near The distance of the near plane. Marks how close geometry can get to the
     *  camera before disappearing. Should be a small, but greater than or equal to 1.
     * \param far The distance of the far plane. Marks how far geometry can get to be away
     *  before it stops rendering. Should be something large, like 1000. */
    void makePerspective(Radian fov, Real n, Real f);

    /*! Sets the projection type of the frustum to perspective. Any world objects not
     *  contained in the described frustum will NOT render.
     * \param width The width of the parent render target (To calculate screen ratio).
     * \param height The height of the parent render target (To calculate screen ratio).
     * \param fov The field of view for the frustum. Wider means more is visible.
     * \param near The distance of the near plane. Marks how close geometry can get to the
     *  camera before disappearing. Should be a small, but greater than or equal to 1.
     * \param far The distance of the far plane. Marks how far geometry can get to be away
     *  before it stops rendering. Should be something large, like 1000. */
    void makePerspective(int width, int height, Radian fov, Real near, Real far);

    /*! Sets the projection type of the frustum to orthographic. Any world objects not
     *  contained in the described frustum will NOT render.
     * \param left The location of the left plane.
     * \param right The location of the right plane.
     * \param bottom The location of the bottom plane.
     * \param top The location of the top plane.
     * \param near The location of the near plane.
     * \param far The location of the far plane. */
    void makeOrtho(Real left, Real right, Real bottom,
                   Real top, Real near = 1.0, Real far = -1.0);

    /*! Sets up an orthographic projection centered at a particular location with a
     *  particular width, with an automatically computed height based on the current
     *  aspect ratio.
     * \param width The width of the frustum
     * \param center The center of the frustum
     * \param near The near clipping plane
     * \param far The far clipping plane */
    void centerOrtho(Real width, const Vector2 &center, Real near, Real far);

    /*! Returns the projection matrix that describes this frustum.
     * \return The projection matrix. */
    const Matrix& getProjectionMatrix();

    friend std::ostream& operator<<(std::ostream &lhs, const ViewFrustum &rhs);

protected:
    /*! Needs to be called when any of the projection matrix parameters change. */
    void updateProjectionMatrix();

    /*! Needs to be called whenever the transformation or projection matrix updates. */
    void updatePlanes();

    //Plane extraction functions.
    void extractLeft(const Matrix &clipping);
    void extractRight(const Matrix &clipping);
    void extractBottom(const Matrix &clipping);
    void extractTop(const Matrix &clipping);
    void extractNear(const Matrix &clipping);
    void extractFar(const Matrix &clipping);

private:
    ProjectionType _projectionType;
    Matrix _transormationMatrix;
    Matrix _projectionMatrix;

    Degree _fov;
    Real _ratio;
    Real _near, _far;
    Real _left, _right;
    Real _bottom, _top;
    
    bool _valid;

};

#endif
