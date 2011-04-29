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

void PathNode::addEdges(const EdgeList &edges) {
    for(ConstEdgeIterator itr = edges.begin(); itr != edges.end(); itr++) {
        addEdge((*itr).first, (*itr).second);
    }
}

// Return true if this edge exists and is deleted, false if it wasn't an edge
bool PathNode::removeEdge(PathNode *otherNode) {
    for(EdgeIterator itr = _edges.begin(); itr != _edges.end(); itr++) {
        if((*itr).first == otherNode) {
            _edges.erase(itr);
            return true;
        }
    }
    return false;
}

const EdgeList &PathNode::getEdges() { return _edges; }
bool PathNode::hasEdge(PathNode *node) {
    for(EdgeIterator itr = _edges.begin(); itr != _edges.end(); itr++) {
        if((*itr).first == node) {
            return true;
        }
    }
    return false;
}

Vector3 PathNode::getLowerCorner() { return _lowerCorner; }
Vector3 PathNode::getUpperCorner() { return _upperCorner; }
Vector3 PathNode::getCenter() { return _lowerCorner + ((_upperCorner - _lowerCorner) / 2.0); }

void PathNode::setLowerCorner(const Vector3 &corner) { _lowerCorner = corner; }
void PathNode::setUpperCorner(const Vector3 &corner) { _upperCorner = corner; }

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
    for(NodeIterator itr = _nodes->begin(); itr != _nodes->end(); itr++) {
        if(*itr != NULL) { deleteNode(*itr); }
    }
    delete _nodes;
}

Vector3 PathManager::getDimensions() const { return _dimensions; }

NodeList *PathManager::getNodes() const { return _nodes; }

// Returns true if a change was made that requires nodes to be regrouped
void PathManager::setNodeType(int x, int y, int z, NodeType type) {
    PathNode *thisNode = getNode(x,y,z);

    // If the types are the same, there's no work to be done, bail out
    NodeType oldType = thisNode->getType();
    if(oldType == type) { return; }

    // Get the dimensions of this node
    Vector3 lowerCorner = thisNode->getLowerCorner(),
            upperCorner = thisNode->getUpperCorner();

    // Delete the existing node data
    deleteNode(thisNode);

    // Create (a) new node(s) in place of the old one
    for(int sX = lowerCorner[0]; sX <= upperCorner[0]; sX++) {
        for(int sY = lowerCorner[1]; sY <= upperCorner[1]; sY++) {
            for(int sZ = lowerCorner[2]; sZ <= upperCorner[2]; sZ++) {
                Vector3 position = Vector3(sX,sY,sZ);
                setNode(sX,sY,sZ,new PathNode(position, position, ((sX != x) || (sY != y) || (sZ != z)) ? oldType : type));
            }
        }
    }

    // Add edges to each of the new nodes
    for(int sX = lowerCorner[0]; sX <= upperCorner[0]; sX++) {
        for(int sY = lowerCorner[1]; sY <= upperCorner[1]; sY++) {
            for(int sZ = lowerCorner[2]; sZ <= upperCorner[2]; sZ++) {
                addEdgesFor(sX, sY, sZ);
            }
        }
    }

    // Regroup the nodes affected
    //regroupNodes(lowerCorner, upperCorner);

    // Super slow debug validation code
    //ASSERT(verifyEdges());
}

void PathManager::addEdgesFor(int x, int y, int z) {
    PathNode *thisNode = getNode(x,y,z);
    if(thisNode->getType() != PATHABLE) { return; }

    for(int i=-1; i<=1; i++) {
        for(int j=-1; j<=1; j++) {
            int local_x = x+i,
                local_y = y+j,
                local_z = z;
            if(local_x < 0 || local_x >= _dimensions[0] || local_y < 0 || local_y >= _dimensions[1]) { continue; }

            bool isDiagonal = (abs(i) == abs(j));
            PathWeight weight = (isDiagonal ? DIAGONAL_WEIGHT : CARDINAL_WEIGHT);

            PathNode *edgeNode = getNode(local_x, local_y, local_z);
            if(edgeNode->getType() != PATHABLE) {
                if(edgeNode->getType() == CLOSED) { local_z += 1; }
                else                              { local_z -= 1; }
                weight = (isDiagonal ? CORNER_WEIGHT : DIAGONAL_WEIGHT);
                if(local_z < 0 || local_z >= _dimensions[2]) { continue; }

                edgeNode = getNode(local_x, local_y, local_z);
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
    bool done = false;
    while(!done) {
        done = true;

        std::list<PathNode*> visitedNodes;
        for(int x = lowerCorner.x; x <= upperCorner.x; x++) {
            for(int y = lowerCorner.y; y <= upperCorner.y; y++) {
                for(int z = lowerCorner.z; z <= upperCorner.z; z++) {
                    PathNode *thisNode = getNode(x,y,z);

                    // Skip any nodes that have already been collapsed as much as they can
                    std::list<PathNode*>::iterator itr = visitedNodes.begin();
                    for(; itr != visitedNodes.end(); itr++) { if(thisNode == (*itr)) { break; } }

                    if(itr == visitedNodes.end()) {
                        if(growNode(thisNode, visitedNodes)) {
                            // Node was collapsed update the corners to accurately reflect the new size
                            if(lowerCorner < thisNode->getLowerCorner()) { lowerCorner = thisNode->getLowerCorner(); }
                            if(upperCorner < thisNode->getUpperCorner()) { upperCorner = thisNode->getUpperCorner(); }
                        }
                    }
                }
            }
        }
    }
}

bool PathManager::growNode(PathNode *thisNode, std::list<PathNode*> &visitedNodes) {
    bool done = false, collapsed = false;
    while(!done) {
        done = true;

        Vector3 thisLower = thisNode->getLowerCorner(),
                thisUpper = thisNode->getUpperCorner();

        // Check each connected node to see if these nodes can collapse
        EdgeList edges = thisNode->getEdges();
        EdgeIterator edgeItr = edges.begin();
        for(; edgeItr != edges.end(); edgeItr++) {
            PathNode *connectedNode = (*edgeItr).first;
            Vector3 connectedLower = connectedNode->getLowerCorner(),
                    connectedUpper = connectedNode->getUpperCorner();

            // Determine which axes have shared positions and dimensions
            char setAxes = 0;
            Vector3 newLower, newUpper;
            if(thisLower.x == connectedLower.x && thisUpper.x == connectedUpper.x) {
                setAxes |= 0x1;
                newLower.x = thisLower.x;
                newUpper.x = thisUpper.x;
            }
            if(thisLower.y == connectedLower.y && thisUpper.y == connectedUpper.y) {
                setAxes |= 0x2;
                newLower.y = thisLower.y;
                newUpper.y = thisUpper.y;
            }
            if(thisLower.z == connectedLower.z && thisUpper.z == connectedUpper.z) {
                setAxes |= 0x4;
                newLower.z = thisLower.z;
                newUpper.z = thisUpper.z;
            }

            if(setAxes == 0x3) {
                // X/Y plane is shared, collapse is possible
                newLower.z = Math::Min(thisLower.z, connectedLower.z);
                newUpper.z = Math::Max(thisUpper.z, connectedUpper.z);
            } else if(setAxes == 0x5) {
                // X/Z plane is shared, collapse is possible
                newLower.y = Math::Min(thisLower.y, connectedLower.y);
                newUpper.y = Math::Max(thisUpper.y, connectedUpper.y);
            } else if(setAxes == 0x6) {
                // Y/Z plane is shared, collapse is possible
                newLower.x = Math::Min(thisLower.x, connectedLower.x);
                newUpper.x = Math::Max(thisUpper.x, connectedUpper.x);
            } else {
                // Collapse not possible
                continue;
            }

            Info("Joining node " << thisNode->getLowerCorner() << " (" << (thisNode->getUpperCorner() - thisNode->getLowerCorner()) << ") with " << connectedNode->getLowerCorner() << " (" << (connectedNode->getUpperCorner() - connectedNode->getLowerCorner()) << ")");
            done = false;
            collapsed = true;

            // Remove the connected node from the visited list (if it exists there)
            visitedNodes.remove(connectedNode);

            // Collapse the two nodes
            collapseNodes(thisNode, connectedNode);

            // Expand this node to encapsulate the connected node
            thisNode->setLowerCorner(newLower);
            thisNode->setUpperCorner(newUpper);

            // Break out, start the loop over again with the new edges
            break;
        }
    }

    return collapsed;
}

void PathManager::collapseNodes(PathNode *host, PathNode *guest) {
    // Add the edges of the connected node to this one, removing internal edges as necessary
    Info("Removing " << host << " from " << guest << "'s edges");
    guest->removeEdge(host);
    host->addEdges(guest->getEdges());
    Info("Removing " << guest << " from " << host << "'s edges");
    host->removeEdge(guest);

    // Add this node's edges to the connected node's neighbors
    EdgeList edges = guest->getEdges();
    ConstEdgeIterator itr = edges.begin();
    for(; itr != edges.end(); itr++) {
        if((*itr).first == host) { continue; }
        (*itr).first->addEdge(host, (*itr).second);
    }

    // Delete the old node and its pointers, replacing it with this one (along with any edges that used to point to it)
    deleteNode(guest);
}

// Used to create a new node and set pointers where appropriate
void PathManager::createNode(Vector3 lowerCorner, Vector3 upperCorner, NodeType type) {
    PathNode *newNode = new PathNode(lowerCorner, upperCorner, type);

    for(int x = lowerCorner[0]; x <= upperCorner[0]; x++) {
        for(int y = lowerCorner[1]; y <= upperCorner[1]; y++) {
            for(int z = lowerCorner[2]; z <= upperCorner[2]; z++) {
                int nodeIndex = getIndex(x,y,z);
                if((*_nodes)[nodeIndex] != NULL) { deleteNode((*_nodes)[nodeIndex]); }
                (*_nodes)[nodeIndex] = newNode;
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
        Info("Removing edge " << (*itr).first << " from " << node);
        bool result = (*itr).first->removeEdge(node);
        ASSERT(result);
    }

    // Null out all the pointers to the node
    for(int x = lowerCorner[0]; x <= upperCorner[0]; x++) {
        for(int y = lowerCorner[1]; y <= upperCorner[1]; y++) {
            for(int z = lowerCorner[2]; z <= upperCorner[2]; z++) {
                setNode(x,y,z,NULL);
            }
        }
    }

    delete node;
}

inline int PathManager::getIndex(int x, int y, int z) {
    return ((z*_dimensions[0]*_dimensions[1])+(y*_dimensions[0])+x);
}

inline PathNode *PathManager::getNode(int x, int y, int z) {
    return (*_nodes)[getIndex(x, y, z)];
}

inline void PathManager::setNode(int x, int y, int z, PathNode *node) {
    (*_nodes)[getIndex(x,y,z)] = node;
}

bool PathManager::verifyEdges() {
    NodeList visited;
    for(int x=0; x<_dimensions[0]; x++) {
        for(int y=0; y<_dimensions[1]; y++) {
            for(int z=0; z<_dimensions[2]; z++) {
                PathNode *thisNode = getNode(x,y,z);
                NodeIterator itr = visited.begin();
                for(; itr != visited.end(); itr++) {
                    if((*itr) == thisNode) { break; }
                }
                if(itr == visited.end()) {
                    EdgeList edges = thisNode->getEdges();
                    ConstEdgeIterator eItr = edges.begin();
                    for(; eItr != edges.end(); eItr++) {
                        if(!(*eItr).first->hasEdge(thisNode)) {
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}
