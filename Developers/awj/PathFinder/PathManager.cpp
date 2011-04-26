#include "PathManager.h"

// ==================
// PathNode Functions
// ==================

PathNode::PathNode(Vector3 lowerCorner, Vector3 upperCorner, NodeType type):
    _lowerCorner(lowerCorner), _upperCorner(upperCorner), _type(type)
{}

PathNode::~PathNode() {}

void PathNode::setType(NodeType type) { _type = type; }
NodeType PathNode::getType() { return _type; }

// Return false if this edge already exists (possibly updating the weight),
//  true if this edge is added as a new edge
bool PathNode::addEdge(PathNode *otherNode, PathWeight weight) {
    for(EdgeIterator itr = _edges.begin(); itr != _edges.end(); itr++) {
        if((*itr).first == otherNode) {
            (*itr).second = weight;
            return false;
        }
    }
    _edges.push_back(std::make_pair(otherNode, weight));
    return true;
}

// Return true if this edge exists and is deleted, false if it wasn't an edge
bool PathNode::removeEdge(PathNode *otherNode) {
    for(EdgeIterator itr = _edges.begin(); itr != _edges.end(); itr++) {
        if((*itr) == otherNode) {
            _edges.erase(itr);
            return true;
        }
    }
    return false;
}

const EdgeList &PathNode::getEdges() { return _edges; }

Vector3 PathNode::getLowerCorner() { return _lowerCorner; }
Vector3 PathNode::getUpperCorner() { return _upperCorner; }
Vector3 PathNode::getCenter() { return _lowerCorner + ((_upperCorner - _lowerCorner) / 2.0); }

bool PathNode::contains(Vector3 point) {
    if(point[0] < _lowerCorner[0] || point[0] > _upperCorner[0] ||
       point[1] < _lowerCorner[1] || point[1] > _upperCorner[1] ||
       point[2] < _lowerCorner[2] || point[2] > _upperCorner[2]) { return false; }
    else                                                         { return true; }
}

// =====================
// PathManager Functions
// =====================

PathManager::PathManager(Vector3 dimensions):
    _dimensions(dimensions)
{
    _nodes = new NodeList(_dimensions[0] * _dimensions[1] * _dimensions[2]);
    createNode(Vector3(0,0,0), _dimensions - Vector3(1,1,1), CLOSED);
}

PathManager::~PathManager() {
    for(NodeIterator itr = _nodes.begin(); itr != _nodes.end(); itr++) {
        if(*itr != NULL) { deleteNode(*itr); }
    }
    delete _nodes;
}

// Returns true if a change was made that requires nodes to be regrouped
bool PathManager::setNodeType(int x, int y, int z, NodeType type) {
    PathNode *thisNode = NODE(x,y,z);

    // If the types are the same, there's no work to be done, bail out
    NodeType oldType = thisNode->getType();
    if(oldType == type) { return false; }

    // Get the dimensions of this node
    Vector3 lowerCorner = node->getLowerCorner(),
            upperCorner = node->getUpperCorner();

    // Delete the existing node data
    deleteNode(thisNode);

    // Create (a) new node(s) in place of the old one
    for(int sX = lowerCorner[0]; sX <= upperCorner[0]; sX++) {
        for(int SY = lowerCorner[1]; sY <= upperCorner[1]; sY++) {
            for(int sZ = lowerCorner[2]; sZ <= upperCorner[2]; sZ++) {
                Vector3 position = Vector3(sX,sY,sZ);
                NODE(sX,sY,sZ) = new PathNode(position, position, ((sX != x) || (sY != y) || (sZ != z)) ? oldType : type);
            }
        }
    }

    // Add edges to each of the new nodes
    for(int sX = lowerCorner[0]; sX <= upperCorner[0]; sX++) {
        for(int SY = lowerCorner[1]; sY <= upperCorner[1]; sY++) {
            for(int sZ = lowerCorner[2]; sZ <= upperCorner[2]; sZ++) {
                addEdgesFor(sX, sY, sZ);
            }
        }
    }

    // Regroup the nodes affected
    regroupNodes(lowerCorner, upperCorner);
}

void PathManager::addEdgesFor(int x, int y, int z) {
    PathNode *thisNode = NODE(x,y,z);
    if(thisNode->getType() != PATHABLE) { return; }

    for(int i=-1; i<=1; i++) {
        for(int j=-1; j<=1; j++) {
            int local_x = x+i,
                local_y = y+j,
                local_z = z;
            if(local_x < 0 || local_x >= _dimensions[0] || local_y < 0 || local_y >= _dimensions[1]) { continue; }

            bool isDiagonal = (abs(i) == abs(j));
            PathWeight weight = (isDiagonal ? DIAGONAL_WEIGHT : CARDINAL_WEIGHT);

            PathNode *edgeNode = NODE(local_x, local_y, local_z);
            if(edgeNode->getType() != PATHABLE) {
                if(edgeNode->getType() == CLOSED) { local_z += 1; }
                else                              { local_z -= 1; }
                weight = (isDiagonal ? CORNER_WEIGHT : DIAGONAL_WEIGHT);
                if(local_z < 0 || local_z >= _dimensions[2]) { continue; }

                edgeNode = NODE(local_x, local_y, local_z);
                if(edgeNode->getType() != PATHABLE) { continue; }
            }

            edgeNode->addEdge(thisNode, weight);
            thisNode->addEdge(edgeNode, weight);
        }
    }
}

// Accepts two vectors as boundaries for nodes that need grouping;
//  the nodes contained within and the immediately adjacent nodes will be examined,
//  expanding the are to be grouped if nodes are grouped with nodes outside the area
void PathManager::regroupNodes(Vector3 lowerCorner, Vector3 upperCorner) {
    // TODO - Write this function
}

// Used to create a new node and set pointers where appropriate
void PathManager::createNode(Vector3 lowerCorner, Vector3 upperCorner, NodeType type) {
    PathNode *newNode = new PathNode(lowerCorner, upperCorner, type);

    for(int x = lowerCorner[0]; x <= upperCorner[0]; x++) {
        for(int y = lowerCorner[1]; y <= upperCorner[1]; y++) {
            for(int z = lowerCorner[2]; z <= upperCorner[2]; z++) {
                int nodeIndex = NODE_INDEX(x,y,z);
                if(_nodes[nodeIndex] != NULL) { deleteNode(_nodes[nodeIndex]); }
                _nodes[nodeIndex] = newNode;
            }
        }
    }
}

// Used to delete a node's memory and NULL any pointers that remain to that node in the graph
//  (nice way to only delete a node once when it spans many x,y,z triplets)
void PathManager::deleteNode(PathNode *node) {
    Vector3 lowerCorner = node->getLowerCorner(),
            upperCorner = node->getUpperCorner();

    // Remove all edges to this node
    const EdgeList edgeList = node->getEdges();
    for(ConstEdgeIterator itr = edgeList.begin(); itr != edgeList.end(); itr++) {
        (*itr).first->removeEdge(node);
    }

    // Null out all the pointers to the node
    for(int x = lowerCorner[0]; x <= upperCorner[0]; x++) {
        for(int y = lowerCorner[1]; y <= upperCorner[1]; y++) {
            for(int z = lowerCorner[2]; z <= upperCorner[2]; z++) {
                NODE(x,y,z) = NULL;
            }
        }
    }

    delete node;
}
