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

void PathVisualizer::clear() {
    // Clean out old geometry
    clearRenderables();
}

void PathVisualizer::update(bool drawNodes, bool drawEdges) {
    // Prepare new geometry
    std::vector<Vector3> pathVerts;
    std::vector<unsigned int> pathIndices;

    // Iterate over each node
    NodeList *nodes = _pathManager->getNodes();
    NodeIterator itr = nodes->begin();
    for(int c = 0; itr != nodes->end(); itr++, c++) {
        PathNode *thisNode = (*itr);
        if(thisNode->getType() != PATHABLE) { continue; }

        // Add geometry for the node itself
        if(drawNodes) {
            RenderOperation *boxOp = RenderOperation::CreateBoxOp(thisNode->getLowerCorner(), thisNode->getUpperCorner() + Vector3(1,1,1), true);
            addRenderable(new Renderable(boxOp, Content::GetOrLoad<Material>("red")));
        }

        // Add geometry for this node's edges
        if(drawEdges) {
            Vector3 offset(0.5, 0.5, 0.5);

            // First, add this node's location as a vertex
            int thisIndex = pathVerts.size();
            pathVerts.push_back(thisNode->getCenter() + offset);

            const EdgeList edges = thisNode->getEdges();
            ConstEdgeIterator eItr = edges.begin();
            for(int c=1; eItr != edges.end(); eItr++, c++) {
                pathVerts.push_back((*eItr).first->getCenter() + offset);
                pathIndices.push_back(thisIndex);
                pathIndices.push_back(thisIndex+c);
            }
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

void PathVisualizer::addPath(const std::vector<Vector3> &path) {
    std::vector<Vector3> pathVerts;
    std::vector<unsigned int> pathIndices;

    Vector3 offset(0.5, 0.5, 0.55); // Slightly above the path edges, for better visibility

    std::vector<Vector3>::const_iterator itr = path.begin();

    if(itr == path.end()) {
        Info("No visualization necessary for an empty path.");
        return;
    }

    // Add the starting vertex
    pathVerts.push_back((*itr) + offset);
    itr++;

    for(int c=1; itr != path.end(); c++, itr++) {
        pathVerts.push_back((*itr) + offset);
        pathIndices.push_back(c-1);
        pathIndices.push_back(c);
    }

    // Prepare buffers
    VertexArray *vertexArray = new VertexArray();
    vertexArray->setPositionBuffer(new PositionBuffer(GL_STATIC_DRAW, GL_FLOAT, 3, pathVerts.size(), &pathVerts[0]));
    IndexBuffer *indexBuffer = new IndexBuffer(GL_STATIC_DRAW, GL_UNSIGNED_INT, pathIndices.size(), &pathIndices[0]);

    // Create the renderable and add it
    RenderOperation *pathOp = new RenderOperation(LINES, vertexArray, indexBuffer);
    Renderable *pathRenderable = new Renderable(pathOp, Content::GetOrLoad<Material>("blue"));
    addRenderable(pathRenderable);
}
