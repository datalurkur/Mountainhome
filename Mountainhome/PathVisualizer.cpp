#include "PathVisualizer.h"
#include <Render/VertexArray.h>
#include <Render/IndexBuffer.h>
#include <Content/Content.h>

PathVisualizer::PathVisualizer(PathManager *pathManager): Entity("PathVisualizer"), _pathManager(pathManager) {
    // Set the bounding box
    AABB3 localAABB;
    localAABB.setMinMax(Vector3(0,0,0), Vector3(pathManager->getDimensions()));
    expandLocalAABB(localAABB);
}

PathVisualizer::~PathVisualizer() {}

void PathVisualizer::update()
{
    // Clean out old geometry
    clearRenderables();

    // Prepare new geometry
    std::vector<Vector3> pathVerts;
    std::vector<unsigned int> pathIndices;

    // Iterate over each node
    NodeList *nodes = _pathManager->getNodes();
    for(NodeIterator itr = nodes->begin(); itr != nodes->end(); itr++) {
        PathNode *thisNode = (*itr);
        if(thisNode->getType() != PATHABLE) { continue; }

        // Add geometry for the node itself
        RenderOperation *boxOp = RenderOperation::CreateBoxOp(thisNode->getLowerCorner(), thisNode->getUpperCorner() + Vector3(1,1,1), true);
        addRenderable(new Renderable(boxOp, Content::GetOrLoad<Material>("red")));

        // Add geometry for this node's edges
        Vector3 offset(0.5, 0.5, 0.5);

        // First, add this node's location as a vertex
        int thisIndex = pathVerts.size();
        pathVerts.push_back(thisNode->getCenter());

        const EdgeList edges = thisNode->getEdges();
        ConstEdgeIterator eItr = edges.begin();
        for(int c=0; eItr != edges.end(); eItr++, c++) {
            pathVerts.push_back((*eItr).first->getCenter() + offset);
            pathIndices.push_back(thisIndex);
            pathIndices.push_back(thisIndex+c);
        }
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

