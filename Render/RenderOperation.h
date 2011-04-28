/*
 *  RenderOperation.h
 *  Render
 *
 *  Created by loch on 12/14/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _RENDEROPERATION_H_
#define _RENDEROPERATION_H_
#include <Base/Vector.h>
#include "Render.h"

class VertexArray;
class IndexBuffer;

/*! This is the most basic type needed to send vertices down to the rendering hardware.
 *  Nothing about how this data is used is considered here; see Shader for details on
 *  this.
 * \note See Renderable for a more complete explanation of how rendering works.
 * \note This class is separate from Renderable because it is often desireable for many
 *  Renderables to share a single RenderOperation. For example, there may be hundereds of
 *  bounding sphere renderables being rendered, but the vertex data associated with them
 *  only needs to exist once.
 * \note RenderOperation destroys any child VertexArray or IndexBuffer on deconstruction.
 * \seealso Renderable */
class RenderOperation {
public:
    static RenderOperation * CreateSphereOp(int strips, int panels, Real radius, bool wire = false);

    static RenderOperation * CreateBoxOp(const Vector3 &dimensions, bool wire = false);

    static RenderOperation * CreateBoxOp(const Vector3 &start, const Vector3 &end, bool wire = false);

    static RenderOperation * CreateRectangleOp(const Vector2 &dimensions, bool wire = false);

    static RenderOperation * CreateRectangleOp(const Vector2 &start, const Vector2 &end, bool wire = false);

    static RenderOperation * CreateNoOp();

private:
    static void GenerateSphereGeometry(
        int strips, int panels, Real radius, bool wire,
        std::vector<unsigned short>&indices,
        std::vector<Vector3> &vertices,
        std::vector<Vector3> &normals);

public:
    RenderOperation(PrimitiveType type, VertexArray *vertices, IndexBuffer *indices = NULL);
    ~RenderOperation();

    PrimitiveType getPrimitiveType();
    VertexArray * getVertexArray();
    IndexBuffer * getIndexBuffer();

    unsigned int getPrimitiveCount();
    unsigned int getVertexCount();

private:
    friend class RenderContext;
    void render();

private:
    PrimitiveType _type;
    VertexArray *_vertices;
    IndexBuffer *_indices;

};

#endif
