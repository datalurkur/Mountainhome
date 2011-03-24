/*
 *  RenderOperation.cpp
 *  Render
 *
 *  Created by loch on 12/14/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "RenderOperation.h"
#include "GL_Helper.h"

#include "VertexArray.h"
#include "IndexBuffer.h"

RenderOperation * RenderOperation::CreateNoOp() {
    return new RenderOperation(TRIANGLES, NULL);
}

RenderOperation * RenderOperation::CreateBoxOp(const Vector3 &dimensions, bool wire) {
    Vector3 radius = dimensions / 2.0;

    if (wire) {
        std::vector<Vector3> positions(8);

        // BOTTOM
        positions[0] = Vector3( radius.x, -radius.y,  radius.z);
        positions[1] = Vector3(-radius.x, -radius.y,  radius.z);
        positions[2] = Vector3(-radius.x, -radius.y, -radius.z);
        positions[3] = Vector3( radius.x, -radius.y, -radius.z);

        // TOP
        positions[4] = Vector3( radius.x, radius.y,  radius.z);
        positions[5] = Vector3(-radius.x, radius.y,  radius.z);
        positions[6] = Vector3(-radius.x, radius.y, -radius.z);
        positions[7] = Vector3( radius.x, radius.y, -radius.z);

        std::vector<short> indices(24);

        for (int i = 0; i < 8; i++) {
            int index = ((i + 1) / 2);
            indices[i +  0] = (index % 4) + 0;                // BOTTOM
            indices[i +  8] = (index % 4) + 4;                // TOP
            indices[i + 16] = index + ((i % 2) == 0 ? 0 : 3); // SIDES
        }

        VertexArray *vertexArray = new VertexArray();
        vertexArray->setPositionBuffer(new PositionBuffer(
            GL_STATIC_DRAW, GL_FLOAT, 3, positions.size(), &positions[0]));

        IndexBuffer *indexBuffer = new IndexBuffer(GL_STATIC_DRAW, GL_UNSIGNED_SHORT, indices.size(), &indices[0]);

        return new RenderOperation(LINES, vertexArray, indexBuffer);
    } else {
        std::vector<Vector3> positions(24);

        // BACK
        positions[ 0] = Vector3( radius.x, -radius.y, -radius.z);
        positions[ 1] = Vector3(-radius.x, -radius.y, -radius.z);
        positions[ 2] = Vector3(-radius.x,  radius.y, -radius.z);
        positions[ 3] = Vector3( radius.x,  radius.y, -radius.z);

        // FRONT
        positions[ 4] = Vector3(-radius.x, -radius.y, radius.z);
        positions[ 5] = Vector3( radius.x, -radius.y, radius.z);
        positions[ 6] = Vector3( radius.x,  radius.y, radius.z);
        positions[ 7] = Vector3(-radius.x,  radius.y, radius.z);

        // LEFT
        positions[ 8] = Vector3(-radius.x, -radius.y, -radius.z);
        positions[ 9] = Vector3(-radius.x, -radius.y,  radius.z);
        positions[10] = Vector3(-radius.x,  radius.y,  radius.z);
        positions[11] = Vector3(-radius.x,  radius.y, -radius.z);

        // RIGHT
        positions[12] = Vector3(radius.x, -radius.y,  radius.z);
        positions[13] = Vector3(radius.x, -radius.y, -radius.z);
        positions[14] = Vector3(radius.x,  radius.y, -radius.z);
        positions[15] = Vector3(radius.x,  radius.y,  radius.z);

        // BOTTOM
        positions[16] = Vector3( radius.x, -radius.y,  radius.z);
        positions[17] = Vector3(-radius.x, -radius.y,  radius.z);
        positions[18] = Vector3(-radius.x, -radius.y, -radius.z);
        positions[19] = Vector3( radius.x, -radius.y, -radius.z);

        // TOP
        positions[20] = Vector3(-radius.x, radius.y,  radius.z);
        positions[21] = Vector3( radius.x, radius.y,  radius.z);
        positions[22] = Vector3( radius.x, radius.y, -radius.z);
        positions[23] = Vector3(-radius.x, radius.y, -radius.z);

        std::vector<Vector3> normals(24);
        // BACK
        normals[ 0] = Vector3(0, 0, -1);
        normals[ 1] = Vector3(0, 0, -1);
        normals[ 2] = Vector3(0, 0, -1);
        normals[ 3] = Vector3(0, 0, -1);

        // FRONT
        normals[ 4] = Vector3(0, 0, 1);
        normals[ 5] = Vector3(0, 0, 1);
        normals[ 6] = Vector3(0, 0, 1);
        normals[ 7] = Vector3(0, 0, 1);

        // LEFT
        normals[ 8] = Vector3(-1, 0, 0);
        normals[ 9] = Vector3(-1, 0, 0);
        normals[10] = Vector3(-1, 0, 0);
        normals[11] = Vector3(-1, 0, 0);

        // RIGHT
        normals[12] = Vector3(1, 0, 0);
        normals[13] = Vector3(1, 0, 0);
        normals[14] = Vector3(1, 0, 0);
        normals[15] = Vector3(1, 0, 0);

        // BOTTOM
        normals[16] = Vector3(0, -1, 0);
        normals[17] = Vector3(0, -1, 0);
        normals[18] = Vector3(0, -1, 0);
        normals[19] = Vector3(0, -1, 0);

        // TOP
        normals[20] = Vector3(0, 1, 0);
        normals[21] = Vector3(0, 1, 0);
        normals[22] = Vector3(0, 1, 0);
        normals[23] = Vector3(0, 1, 0);

        std::vector<Vector2> texcoords(24);
        // Mirror the texture the same on every face.
        for (int i = 0; i < 6; i++) {
            texcoords[(i * 4) + 0] = Vector2(0, 0);
            texcoords[(i * 4) + 1] = Vector2(1, 0);
            texcoords[(i * 4) + 2] = Vector2(1, 1);
            texcoords[(i * 4) + 3] = Vector2(0, 1);
        }

        VertexArray *vertexArray = new VertexArray();
        vertexArray->setPositionBuffer(new PositionBuffer(
            GL_STATIC_DRAW, GL_FLOAT, 3, positions.size(), &positions[0]));
        vertexArray->setNormalBuffer(new NormalBuffer(
            GL_STATIC_DRAW, GL_FLOAT, normals.size(), &normals[0]));
        vertexArray->setTexCoordBuffer(0, new TexCoordBuffer(
            GL_STATIC_DRAW, GL_FLOAT, 2, texcoords.size(), &texcoords[0]));

        return new RenderOperation(QUADS, vertexArray);
    }
}

RenderOperation * RenderOperation::CreateRectangleOp(const Vector2 &dimensions, bool wire) {
    return CreateRectangleOp(dimensions, Vector2(0,0), wire);
}

RenderOperation * RenderOperation::CreateRectangleOp(const Vector2 &dimensions, const Vector2 &offsets, bool wire) {
    Vector2 radius = dimensions / 2.0;

    // Generate the sphere geometry.
    std::vector<Vector3> positions(4);
    positions[0] = Vector3(-radius.x + offsets.x, -radius.y + offsets.y, 0);
    positions[1] = Vector3( radius.x + offsets.x, -radius.y + offsets.y, 0);
    positions[2] = Vector3( radius.x + offsets.x,  radius.y + offsets.y, 0);
    positions[3] = Vector3(-radius.x + offsets.x,  radius.y + offsets.y, 0);

    std::vector<Vector3> normals(4);
    normals[0] = Vector3(0, 0, 1);
    normals[1] = Vector3(0, 0, 1);
    normals[2] = Vector3(0, 0, 1);
    normals[3] = Vector3(0, 0, 1);

    std::vector<Vector2> texcoords(4);
    texcoords[0] = Vector2(0, 0);
    texcoords[1] = Vector2(1, 0);
    texcoords[2] = Vector2(1, 1);
    texcoords[3] = Vector2(0, 1);

    VertexArray *vertexArray = new VertexArray();
    vertexArray->setPositionBuffer(new PositionBuffer(
        GL_STATIC_DRAW, GL_FLOAT, 3, positions.size(), &positions[0]));
    vertexArray->setNormalBuffer(new NormalBuffer(
        GL_STATIC_DRAW, GL_FLOAT, normals.size(), &normals[0]));
    vertexArray->setTexCoordBuffer(0, new TexCoordBuffer(
        GL_STATIC_DRAW, GL_FLOAT, 2, texcoords.size(), &texcoords[0]));

    return new RenderOperation(wire ? LINES : QUADS, vertexArray);
}

RenderOperation * RenderOperation::CreateSphereOp(int strips, int panels, Real radius, bool wire) {
    // return CreateBoxOp(Vector3(radius,radius,radius), wire);

    // Generate the sphere geometry.
    std::vector<Vector3> positions;
    std::vector<Vector3> normals;
    std::vector<unsigned short> indices;

    GenerateSphereGeometry(strips, panels, radius, wire, indices, positions, normals);

    VertexArray *vertexArray = new VertexArray();
    vertexArray->setPositionBuffer(new PositionBuffer(
        GL_STATIC_DRAW, GL_FLOAT, 3, positions.size(), &positions[0]));
    vertexArray->setNormalBuffer(new NormalBuffer(
        GL_STATIC_DRAW, GL_FLOAT, normals.size(), &normals[0]));

    IndexBuffer *indexBuffer = new IndexBuffer(GL_STATIC_DRAW, GL_UNSIGNED_SHORT, indices.size(), &indices[0]);

    return new RenderOperation(
        wire ? LINES : TRIANGLES,
        vertexArray,
        indexBuffer);
}

void RenderOperation::GenerateSphereGeometry(
    int strips, int panels,
    Real radius, bool wire,
    std::vector<unsigned short> &indices,
    std::vector<Vector3> &vertices,
    std::vector<Vector3> &normals)
{
    // XXXBMW: TODO - This function generates what appears to be a bogus strip at the end.
    // We should look into this once we have moving camera again.

    Radian panel_step = Radian(Math::PI * 2.0f / panels);
    Radian strip_step = Radian(Math::PI / strips);

    unsigned short vIndex = 1;
    unsigned short iIndex = 0;

    ASSERT(strips > 1); // Needs at least two strips to form a sphere.
    strips = Math::Max(strips, 2);

    ASSERT(panels > 3); // Needs at least three panels to form a sphere.
    panels = Math::Max(panels, 3);

    // One vert for each strip/panel and the two capping verts.
    vertices.resize((strips * panels) + 2, Vector3(0, 0, 0));
    normals.resize(vertices.size());

    // For the wire case:
    //     Two 2-vert prims per panel on the middle strips.
    //     Three 2-vert prims per panel for the capping strips (top takes 1 per, bottom takes 2 per).
    // For the solid case:
    //     Two 3-vert prims per panel on the middle strips.
    //     One 3-vert prim per panel on the capping strips.
    indices.resize(((strips - 2) * panels * (wire ? 2 : 3) * 2) + (panels * 3 * 2), 0);

    ASSERT((unsigned short)vertices.size() == vertices.size()); // Sphere size is too large to support short type indices.

    // Cap the sphere.
    vertices[0] = Vector3(0, radius, 0);
    normals[0] = Vector3(0, 1, 0);

    vertices[vertices.size() - 1] = Vector3(0, -radius, 0);
    normals[vertices.size() - 1] = Vector3(0, -1, 0);

    for (int strip = 0; strip < strips; strip++)
    {
        float scale = radius * Math::Sin(strip_step * (strip + 1));
        float yPos = radius * Math::Cos(strip_step * (strip + 1));
        unsigned short lastVIndex = (strip + 1) * panels;

        for (int panel = 0; panel < panels; panel++)
        {
            // Add the new vertex.
            vertices[vIndex].x = scale * Math::Cos(panel_step * panel);
            vertices[vIndex].y = yPos;
            vertices[vIndex].z = scale * Math::Sin(panel_step * panel);

            normals[vIndex] = vertices[vIndex].getNormalized();

            // Add indices for the new primitives.
            if (wire)
            {
                if (strip == 0)
                {
                    // Vertical lines.
                    indices[iIndex++] = 0;
                    indices[iIndex++] = vIndex;

                    // Horizontal lines.
                    indices[iIndex++] = lastVIndex;
                    indices[iIndex++] = vIndex;
                }
                else if (strip == strips - 1)
                {
                    // The horizontal lines will be handled by the previous strip.
                    indices[iIndex++] = vertices.size() - 1;
                    indices[iIndex++] = vIndex - panels;
                }
                else
                {
                    // Vertical lines.
                    indices[iIndex++] = vIndex - panels;
                    indices[iIndex++] = vIndex;

                    // Horizontal lines.
                    indices[iIndex++] = lastVIndex;
                    indices[iIndex++] = vIndex;
                }
            }
            else
            {
                if (strip == 0)
                {
                    indices[iIndex++] = 0;
                    indices[iIndex++] = lastVIndex;
                    indices[iIndex++] = vIndex;
                }
                else if (strip == strips - 1)
                {
                    indices[iIndex++] = vertices.size() - 1;
                    indices[iIndex++] = vIndex - panels;
                    indices[iIndex++] = lastVIndex - panels;
                }
                else
                {
                    // Bottom right triangle.
                    indices[iIndex++] = lastVIndex - panels;
                    indices[iIndex++] = lastVIndex;
                    indices[iIndex++] = vIndex;

                    // Top left triangle.
                    indices[iIndex++] = lastVIndex - panels;
                    indices[iIndex++] = vIndex;
                    indices[iIndex++] = vIndex - panels;
                }
            }

            lastVIndex = vIndex++;
        }
    }
}

RenderOperation::RenderOperation(
    PrimitiveType type,
    VertexArray *vertices,
    IndexBuffer *indices
):
    _type(type),
    _vertices(vertices),
    _indices(indices)
{}

RenderOperation::~RenderOperation() {
    if (_vertices) {
        delete _vertices; _vertices = NULL;
    }

    if (_indices) {
        delete _indices; _indices = NULL;
    }
}

unsigned int RenderOperation::getPrimitiveCount() {
    int vertexCountPerPrim;

    switch(_type) {
    case LINE_STRIP:     return Math::Max(0, getVertexCount() - 1);
    case TRIANGLE_STRIP: return Math::Max(0, getVertexCount() - 2);
    case POINTS:         vertexCountPerPrim = 1; break;
    case LINES:          vertexCountPerPrim = 2; break;
    case TRIANGLES:      vertexCountPerPrim = 3; break;
    case QUADS:          vertexCountPerPrim = 4; break;
    default: THROW(InternalError, "Unsupported PrimitiveType");
    }

    return getVertexCount() / vertexCountPerPrim;
}

unsigned int RenderOperation::getVertexCount() {
    if (_indices)  { return _indices->getElementCount();  }
    if (_vertices) { return _vertices->getElementCount(); }
    return 0;
}

PrimitiveType RenderOperation::getPrimitiveType() { 
    return _type;
}

VertexArray * RenderOperation::getVertexArray() {
    return _vertices;
}

IndexBuffer * RenderOperation::getIndexBuffer() {
    return _indices;
}

void RenderOperation::render() {
    // If there are no vertices, this is a NULL operation.
    if (!_vertices) { return; }

    _vertices->enable();

    if (_indices) {
        _indices->render(_type);
    } else {
        glDrawArrays(
            TranslatePrimitiveType(_type),
            0, // offset
            _vertices->getElementCount());
    }

    _vertices->disable();
}
