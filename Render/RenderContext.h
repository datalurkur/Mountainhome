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
#include <Base/Vector.h>
#include <Base/Matrix.h>
#include <Base/AABB.h>

#include "RenderParameterContainer.h"
#include "Renderable.h"
#include "Viewport.h"

class Texture;
class Material;

/*! \brief The render context acts as a wrapper around a system's native rendering API
    \author Brent Wilson
    \date 4/4/07 */
class RenderContext : public RenderParameterContainer {
public:
    RenderContext();
    ~RenderContext();

    void clear(const Color4 &clearColor);

    void render(const Matrix &view, const Matrix &projection, RenderableList &list);

    void render2D(int width, int height, RenderableList &list);

    void renderTexture(Texture *src);

    void setGlobalAmbient(const Color4 &ambientColor);

    void setViewport(const Viewport &viewport);

    const Viewport& getViewport() const;

    /*! Gets the number of Renderables handled since the last resetCounts call. */
    int getRenderableCount() const;

    /*! Gets the number of primitives handled since the last resetCounts call. */
    int getPrimitiveCount() const;

    /*! Gets the number of primitives handled since the last resetCounts call. */
    int getVertexCount() const;

    /*! Resets the Renderable, Primitive, and Vertex counts to zero. */
    void resetCounts();

private:
    void setProjectionMatrix(const Matrix &mat);
    void setModelViewMatrix(const Matrix &mat);

private:
    Viewport _viewport;

    mutable int _renderableCount;
    mutable int _primitiveCount;
    mutable int _vertexCount;

};

#endif
