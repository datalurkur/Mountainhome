/*
 *  RenderContext.h
 *  Engine
 *
 *  Created by Brent Wilson on 4/4/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _RENDERCONTEXT_H_
#define _RENDERCONTEXT_H_
#include "Vector.h"
#include "Matrix.h"
#include "AABB.h"
#include <Render/Material.h>

class Texture;
class RenderTarget;
class Material;

/*! \brief The render context acts as a wrapper around a system's native rendering API
    \todo windows and framebuffers textures and shaders are done via enable calls, but
    not viewports? What is the "good" way to do this? Probably move it all to here in an
    attempt to remove GL calls from as many places as possible.
    \author Brent Wilson
    \date 4/4/07 */
class RenderContext {
public:
    RenderContext();
    ~RenderContext();
	
	void setWireFrame();
	void setFilled();

    void clearBuffers(const Color4 &clearColor) const;
    void renderTexture(Texture *src, RenderTarget *dest);

    void enableAlphaBlend();
    void disableAlphaBlend();

    /// \todo remove the color stuff and add a "setColor"
    void draw2DRect(Real x, Real y, Real w, Real h, const Color3 &color) const;
    void draw2DRect(Real x, Real y, Real w, Real h, const Color4 &color) const;
    void draw2DRect(Real x, Real y, Real w, Real h, Real r, Real g, Real b, Real a) const;
    void draw2DRect(Real x, Real y, Real w, Real h) const;

    void drawBoundingBox(const AABB3 &boundingBox, const Color4 &color);
    void drawTriangles(Vector3 *vertices, int number, const Color4 &color);

    void resetMetrics();
    int getPrimitiveCount() const;
    int getVertexCount() const;
    int getModelCount() const;

    ///\todo Get rid of these!
    void addToPrimitiveCount(int count) const;
    void addToVertexCount(int count) const;
    void addToModelCount(int count) const;

    void setViewport(int x, int y, int width, int height) const;

    void setPerspective(int width, int height, Real fov, Real near, Real far) const;
    void setPerspective(Real fov, Real ratio, Real near, Real far) const;
    void setOrtho2D(Real left, Real right, Real bottom, Real top) const;
    void setOrtho(Real left, Real right, Real bottom,
                  Real top, Real near, Real far) const;
	void setDepthTest(bool setTo);
	void applyLighting(bool setTo);

	const Matrix& getModelviewMatrix();

    void resetModelviewMatrix();
    void setProjectionMatrix(const Matrix &perspective);
	void setViewMatrix(const Matrix &viewMat);
	void setModelMatrix(const Matrix &modelMat);

    void setActiveMaterial(const Material *mat);
    void unsetActiveMaterial(const Material *mat);
    
private:
    mutable int _primitiveCount;
    mutable int _vertexCount;
    mutable int _modelCount;

    Matrix _projectionMatrix;
    Matrix _modelviewMatrix;
    Matrix _modelMatrix;
    Matrix _viewMatrix;
};

#endif
