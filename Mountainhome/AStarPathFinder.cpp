#include "AStarPathFinder.h"

bool compareNodes(AStarNode *first, AStarNode *second) {
    return (first->fCost() < second->fCost());
}

// ===================
// AStarNode Functions
// ===================

AStarNode::AStarNode(PathNode *node, AStarNode *parent, int gCost, int hCost):
    _parent(parent), _pathNode(node), _gCost(gCost), _hCost(hCost)
{}

int AStarNode::fCost() const { return _gCost + _hCost; }
int AStarNode::hCost() const { return _hCost; }

void AStarNode::setGCost(int value) { _gCost = value; }
void AStarNode::setParent(AStarNode *parent) { _parent = parent; }

PathNode *AStarNode::getPathNode() const { return _pathNode; }
AStarNode *AStarNode::getParent() const { return _parent; }

// =========================
// AStarPathFinder Functions
// =========================
AStarPathFinder::AStarPathFinder(Vector3 dimensions): PathManager(dimensions) {}

// Find a path between two vectors, returning the distance traveled for comparison with other paths
int AStarPathFinder::getPath(Vector3 start, Vector3 end, Path &path) {
    //Info("Finding path from " << start << " to " << end);
    // Add the ending node to the open list
    PathNode *firstNode = getNode(end[0], end[1], end[2]);
    _openList.push_back(new AStarNode(firstNode, NULL, 0, distance(start, end)));

    int distance = PATH_NOT_FOUND;
    while(!_openList.empty()) {
        // Remove the node with the lowest F score from the open list and add it to the closed list
        AStarNode *currentNode = _openList.front();
        _closedList.push_back(currentNode);
        _openList.pop_front();

        // Check to see if this node contains the start
        if(currentNode->getPathNode()->contains(start)) {
            distance = fillPath(start, end, currentNode, path);
            _lastPath = path;
            break;
        }

        // Add connected nodes
        EdgeList edges = currentNode->getPathNode()->getEdges();
        ConstEdgeIterator edgeItr = edges.begin();
        for(; edgeItr != edges.end(); edgeItr++) {
            parseConnectedNode(currentNode, (*edgeItr), start);
        }

        // Sort the open list by F score
        _openList.sort(compareNodes);
    }

    // Clean up memory
    clear_list(_openList);
    clear_list(_closedList);

    return distance;
}

const Path &AStarPathFinder::getLastPath() {
    return _lastPath;
}

// Examine the edges of the current node, updating any that already exist in the open list and adding any that don't
void AStarPathFinder::parseConnectedNode(AStarNode *currentNode, const PathEdge &edge, Vector3 goal) {
    PathNode *thisPathNode = edge.first;
    PathWeight thisWeight = edge.second;

    // Skip nodes already in the closed list
    AStarList::iterator closedItr = _closedList.begin();
    for(; closedItr != _closedList.end(); closedItr++) {
        if((*closedItr)->getPathNode() == thisPathNode) { return; }
    }

    // Check to see if this connected node is already in the open list
    AStarList::iterator openItr = _openList.begin();
    for(; openItr != _openList.end(); openItr++) {
        if((*openItr)->getPathNode() == thisPathNode) {
            // Compare the F score of the preexisting open node compared to
            //  the same node with this node as its parent
            if((*openItr)->fCost() > (*openItr)->hCost() + thisWeight) {
                // This path is cheaper, reroute its parent and update its weight
                (*openItr)->setParent(currentNode);
                (*openItr)->setGCost(thisWeight);
            }
            break;
        }
    }

    // We've encountered a node *not* in either list already, add it to the open list
    if(openItr == _openList.end()) {
        _openList.push_back(new AStarNode(thisPathNode, currentNode, thisWeight, distance(thisPathNode->getCenter(), goal)));
    }
}

// Using the current node, walk back up the parent pointers to the terminal node
int AStarPathFinder::fillPath(Vector3 start, Vector3 end, AStarNode *currentNode, Path &path) {
    int distance = currentNode->fCost();

    // Now that we have a list of nodes, we need to compute the positions within those nodes to path to
    // Obviously, begin with the start point
    PathNode *prevPathNode = currentNode->getPathNode();
    path.push_back(start);
    //Info("===================================");
    //Info("Path begins at " << start);

    while((currentNode = currentNode->getParent()) != NULL) {
        PathNode *currentPathNode = currentNode->getPathNode();
        Vector3 currentLower = currentPathNode->getLowerCorner(),
                currentUpper = currentPathNode->getUpperCorner() + Vector3(1,1,1),
                prevLower = prevPathNode->getLowerCorner(),
                prevUpper = prevPathNode->getUpperCorner() + Vector3(1,1,1);

        Vector3 lastPosition = path.back();
        Vector3 nextPositionA, nextPositionB;

        // Determine which axis/axes are clamped due to adjacency information
        char setAxes = 0;
        if(currentLower.x == prevUpper.x) {
            setAxes |= 0x1;
            nextPositionA.x = currentLower.x - 1;
            nextPositionB.x = currentLower.x;
        } else if(currentUpper.x == prevLower.x) {
            setAxes |= 0x1;
            nextPositionA.x = currentUpper.x;
            nextPositionB.x = currentUpper.x - 1;
        }
        if(currentLower.y == prevUpper.y) {
            setAxes |= 0x2;
            nextPositionA.y = currentLower.y - 1;
            nextPositionB.y = currentLower.y;
        } else if(currentUpper.y == prevLower.y) {
            setAxes |= 0x2;
            nextPositionA.y = currentUpper.y;
            nextPositionB.y = currentUpper.y - 1;
        }
        if(currentLower.z == prevUpper.z) {
            setAxes |= 0x4;
            nextPositionA.z = currentLower.z - 1;
            nextPositionB.z = currentLower.z;
        } else if(currentUpper.z == prevLower.z) {
            setAxes |= 0x4;
            nextPositionA.z = currentUpper.z;
            nextPositionB.z = currentUpper.z - 1;
        }

        // Determine interface points for any unset axes
        if(!(setAxes & 0x1)) {
            int minimum = Math::Max(currentLower.x, prevLower.x),
            maximum = Math::Min(currentUpper.x, prevUpper.x);
            nextPositionA.x = (lastPosition.x < minimum) ? minimum : maximum - 1;
            nextPositionB.x = nextPositionA.x;
        }
        if(!(setAxes & 0x2)) {
            int minimum = Math::Max(currentLower.y, prevLower.y),
            maximum = Math::Min(currentUpper.y, prevUpper.y);
            nextPositionA.y = (lastPosition.y < minimum) ? minimum : maximum - 1;
            nextPositionB.y = nextPositionA.y;
        }
        if(!(setAxes & 0x4)) {
            int minimum = Math::Max(currentLower.z, prevLower.z),
            maximum = Math::Min(currentUpper.z, prevUpper.z);
            nextPositionA.z = (lastPosition.z < minimum) ? minimum : maximum - 1;
            nextPositionB.z = nextPositionA.z;
        }

        if(nextPositionA != lastPosition) {
            path.push_back(nextPositionA);
            //Info("[*] " << nextPositionA << " added to path.");
        }
        path.push_back(nextPositionB);
        //Info("[+] " << nextPositionB << " added to path.");

        prevPathNode = currentPathNode;
    }
    return distance;
}

int AStarPathFinder::distance(Vector3 start, Vector3 end) {
    return (start - end).lengthSquared();
}
