#include "PathVisualizer.h"
#include <Render/VertexArray.h>
#include <Render/IndexBuffer.h>
#include <Content/Content.h>

PathVisualizer::PathVisualizer(int width, int height, int depth): Entity("PathVisualizer") {
    // Set the bounding box
    AABB3 localAABB;
    localAABB.setMinMax(Vector3(0,0,0), Vector3(width,height,depth));
    expandLocalAABB(localAABB);
}

PathVisualizer::~PathVisualizer() {}

void PathVisualizer::updateEdges(std::vector<Edge> &edges)
{
    // Clean out old geometry
    clearRenderables();

    // Prepare new geometry
    std::vector<Vector3> pathVerts(edges.size() * 2);;
    std::vector<unsigned int> pathIndices(edges.size() * 2);;

    // Stuff the memory full of path data
    Vector3 offset(0.5, 0.5, 0.5);

    std::vector<Edge>::iterator itr = edges.begin();
    for(int c=0; itr != edges.end(); itr++, c+=2) {
        pathVerts[c]   = (*itr).first + offset;
        pathVerts[c+1] = (*itr).second + offset;
        pathIndices[c]   = c;
        pathIndices[c+1] = c+1;
    }

    // Prepare buffers
    VertexArray *vertexArray = new VertexArray();
    vertexArray->setPositionBuffer(new PositionBuffer(
        GL_STATIC_DRAW, GL_FLOAT, 3, pathVerts.size(), &pathVerts[0]));
    IndexBuffer *indexBuffer = new IndexBuffer(
        GL_STATIC_DRAW, GL_UNSIGNED_INT, pathIndices.size(), &pathIndices[0]);

    // Create the renderable and add it
    RenderOperation *pathOp = new RenderOperation(LINES, vertexArray, indexBuffer);
    Renderable *pathRenderable = new Renderable(pathOp, Content::GetOrLoad<Material>("white"));
    addRenderable(pathRenderable);
}

