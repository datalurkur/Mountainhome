#include "AStarPathFinder.h"

// ===================
// AStarNode Functions
// ===================

AStarNode::AStarNode(AStarNode *parent, PathNode *node, int gCost, int hCost):
    _parent(parent), _pathNode(node), _gCost(gCost), _hCost(hCost)
{}

int AStarNode::fCost() const { return _gCost + _hCost; }
int AStarNode::hCost() const { return _hCost; }

void AStarNode::setGCost(int value) { _gCost = value; }
void AStarNode::setParent(AStarNode *parent) { _parent = parent; }

bool AStarNode::operator<(const AStarNode &other) {
    return (fCost() < other.fCost());
}

PathNode *AStarNode::getPathNode() const { return _pathNode; }
AStarNode *AStarNode::getParent() const { return _parent; }

// =========================
// AStarPathFinder Functions
// =========================
AStarPathFinder::AStarPathFinder(Vector3 dimensions): PathManager(dimensions) {}

// Find a path between two vectors, returning the distance traveled for comparison with other paths
int AStarPathFinder::getPath(Vector3 start, Vector3 end, Path &path) {
    AStarList openList;
    AStarList closedList;

    // Add the ending node to the open list
    openList.push_back(new AStarNode(NULL, getNode(end[0], end[1], end[2]), 0, distance(start, end)));

    int distance = PATH_NOT_FOUND;
    while(!openList.empty()) {
        // Remove the node with the lowest F score from the open list and add it to the closed list
        AStarNode *currentNode = openList.front();
        closedList.push_back(currentNode);
        openList.pop_front();

        // Check to see if this node contains the start
        if(currentNode->getPathNode()->contains(start)) {
            distance = fillPath(start, end, currentNode, path);
            break;
        }

        // Update the open list
        updateOpenList(start, currentNode, openList);

        // Sort the open list by F score
        openList.sort();
    }

    // Clean up memory
    AStarList::iterator itr;
    for(itr = openList.begin(); itr != openList.end(); itr++) { delete (*itr); }
    for(itr = closedList.begin(); itr != closedList.end(); itr++) { delete (*itr); }

    return distance;
}

// Examine the edges of the current node, updating any that already exist in the open list and adding any that don't
void AStarPathFinder::updateOpenList(Vector3 goal, AStarNode *currentNode, AStarList &openList) {
    // Iterate over each of this node's edges
    EdgeList edgeList = currentNode->getPathNode()->getEdges();
    for(ConstEdgeIterator itr = edgeList.begin(); itr != edgeList.end(); itr++) {
        PathNode *thisPathNode = (*itr).first;
        PathWeight thisWeight = (*itr).second;

        // Check to see if this connected node is already in the open list
        AStarList::iterator openItr = openList.begin();
        for(; openItr != openList.end(); itr++) {
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

        // We've encountered a node *not* in the open list already, add it
        if(openItr == openList.end()) {
            openList.push_back(new AStarNode(currentNode, thisPathNode, thisWeight, distance(thisPathNode->getCenter(), goal)));
        }
    }
}

// Using the current node, walk back up the parent pointers to the terminal node
int AStarPathFinder::fillPath(Vector3 start, Vector3 end, AStarNode *currentNode, Path &path) {
    int distance = currentNode->fCost();

    // Now that we have a list of nodes, we need to compute the positions within those nodes to path to
    // Obviously, begin with the start point
    PathNode *prevPathNode = currentNode->getPathNode();
    path.push(start);

    while((currentNode = currentNode->getParent()) != NULL) {
        PathNode *currentPathNode = currentNode->getPathNode();
        Vector3 currentLower = currentPathNode->getLowerCorner(),
                currentUpper = currentPathNode->getUpperCorner(),
                prevLower = prevPathNode->getLowerCorner(),
                prevUpper = prevPathNode->getUpperCorner();

        Vector3 lastPosition = path.top();
        Vector3 nextPosition;

        // Determine which axis the interface of these two nodes lies on
        int interface;
        if(currentLower[0] == prevUpper[0]) {
            nextPosition[0] = currentLower[0];
            interface = 0;
        } else if (currentUpper[0] == prevLower[0]) {
            nextPosition[0] = currentUpper[0];
            interface = 0;
        } else if(currentLower[1] == prevUpper[1]) {
            nextPosition[1] = currentLower[1];
            interface = 1;
        } else if(currentUpper[1] == prevLower[1]) {
            nextPosition[1] = currentUpper[1];
            interface = 1;
        } else if(currentLower[2] == prevUpper[2]) {
            nextPosition[2] = currentLower[2];
            interface = 2;
        } else if(currentUpper[2] == prevLower[2]) {
            nextPosition[2] = currentUpper[2];
            interface = 2;
        } else { ASSERT(0); }

        // Clamp the size of the interface to only the area that is joined, and determine the point closest the previous position
        if(interface != 2) {
            int minZ = (currentLower[2] > prevLower[2]) ? currentLower[2] : prevLower[2],
                maxZ = (currentUpper[2] < prevUpper[2]) ? currentUpper[2] : prevUpper[2];
            nextPosition[2] = (minZ > lastPosition[2]) ? minZ : maxZ;
        }
        if(interface != 1) {
            int minY = (currentLower[1] > prevLower[1]) ? currentLower[1] : prevLower[1],
                maxY = (currentUpper[1] < prevUpper[1]) ? currentUpper[1] : prevUpper[1];
            nextPosition[1] = (minY > lastPosition[1]) ? minY : maxY;
        }
        if(interface != 0) {
            int minX = (currentLower[0] > prevLower[0]) ? currentLower[0] : prevLower[0],
                maxX = (currentUpper[0] < prevUpper[0]) ? currentUpper[0] : prevUpper[0];
            nextPosition[0] = (minX > lastPosition[0]) ? minX : maxX;
        }

        path.push(nextPosition);
    }
    return distance;
}

int AStarPathFinder::distance(Vector3 start, Vector3 end) {
    return (start - end).lengthSquared();
}
