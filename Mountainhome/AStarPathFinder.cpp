#include "AStarPathFinder.h"

bool compareNodes(AStarNode *first, AStarNode *second) {
    return (first->fCost() < second->fCost());
}

// ===================
// AStarNode Functions
// ===================

AStarNode::AStarNode(PathNodeCluster *cluster, Vector3 prePosition, Vector3 postPosition, AStarNode *parent, int gCost, int hCost):
    _parent(parent), _cluster(cluster), _prePosition(prePosition), _postPosition(postPosition), _gCost(gCost), _hCost(hCost)
{}

int AStarNode::fCost() const { return _gCost + _hCost; }
int AStarNode::hCost() const { return _hCost; }

void AStarNode::setGCost(int value) { _gCost = value; }
void AStarNode::setParent(AStarNode *parent) { _parent = parent; }
void AStarNode::setPrePosition(Vector3 position) { _prePosition = position; }

PathNodeCluster *AStarNode::getCluster() const { return _cluster; }
AStarNode *AStarNode::getParent() const { return _parent; }
Vector3 AStarNode::getPrePosition() const { return _prePosition; }
Vector3 AStarNode::getPostPosition() const { return _postPosition; }

// =========================
// AStarPathFinder Functions
// =========================
AStarPathFinder::AStarPathFinder(Vector3 dimensions): PathManager(dimensions) {}

// Find a path between two vectors, returning the distance traveled for comparison with other paths
int AStarPathFinder::getPath(Vector3 start, Vector3 end, Path &path) {
    int distance = PATH_NOT_FOUND;

//    // Add the ending node to the open list
//    PathNodeCluster *firstCluster = getCluster(end[0], end[1], end[2]);
//    if(firstCluster->getType() != PATHABLE) {
//        return distance;
//    }
//
//    _openList.push_back(new AStarNode(firstCluster, end, end, NULL, 0, PathManager::distance(start, end)));
//
//    while(!_openList.empty()) {
//        // Remove the node with the lowest F score from the open list and add it to the closed list
//        AStarNode *currentNode = _openList.front();
//        _closedList.push_back(currentNode);
//        _openList.pop_front();
//
//        PathNodeCluster *currentCluster = currentNode->getCluster();
//
//        // Check to see if this node contains the start
//        if(currentCluster->contains(start)) {
//            distance = currentNode->fCost();
//
//            path.push_back(start);
//            do {
//                Vector3 prePosition = currentNode->getPrePosition(),
//                        postPosition = currentNode->getPostPosition();
//                if(postPosition != path.back()) { path.push_back(postPosition); }
//                if(prePosition != path.back()) { path.push_back(prePosition); }
//            } while((currentNode = currentNode->getParent()) != NULL);
//
//            _lastPath = path;
//            break;
//        }
//
//        // Add connected nodes
//        EdgeList edges = currentCluster->getEdges();
//        ConstEdgeIterator edgeItr = edges.begin();
//        for(; edgeItr != edges.end(); edgeItr++) {
//            PathNodeCluster *edgeCluster;
//            Vector3 edgePosition, prePosition;
//            if((*edgeItr)->clusterA == currentCluster) {
//                edgeCluster = (*edgeItr)->clusterB;
//                edgePosition = (*edgeItr)->nodeB;
//                prePosition = (*edgeItr)->nodeA;
//            } else {
//                // DEBUG
//                ASSERT((*edgeItr)->clusterB == currentCluster);
//                edgeCluster = (*edgeItr)->clusterA;
//                edgePosition = (*edgeItr)->nodeA;
//                prePosition = (*edgeItr)->nodeB;
//            }
//
//            AStarList::iterator itr = _closedList.begin();
//
//            // Skip any nodes already in the closed list
//            for(; itr != _closedList.end(); itr++) {
//                if((*itr)->getCluster() == edgeCluster && (*itr)->getPostPosition() == edgePosition) { break; }
//            }
//            if(itr != _closedList.end()) {
//                continue;
//            }
//
//            // Get the distance between the two points inside of this cluster
//            int interDistance = PathManager::distance(prePosition, currentNode->getPostPosition());
//
//            // Check for existence of this node in the open list
//            itr = _openList.begin();
//            for(; itr != _openList.end(); itr++) {
//                if((*itr)->getCluster() == edgeCluster && (*itr)->getPostPosition() == edgePosition) {
//                    // Is the new route to this location shorter?
//                    if((*itr)->fCost() > (*itr)->hCost() + (*edgeItr)->weight + interDistance) {
//                        (*itr)->setParent(currentNode);
//                        (*itr)->setPrePosition(prePosition);
//                        (*itr)->setGCost((*edgeItr)->weight + interDistance);
//                    }
//                    break;
//                }
//            }
//
//            // Was this node found in neither the open or closed list?
//            if(itr == _openList.end()) {
//                _openList.push_back(new AStarNode(edgeCluster, prePosition, edgePosition, currentNode, (*edgeItr)->weight + interDistance, PathManager::distance(edgePosition, start)));
//            }
//        }
//
//        // Sort the open list by F score
//        _openList.sort(compareNodes);
//    }
//
//    // Clean up memory
//    clear_list(_openList);
//    clear_list(_closedList);

    return distance;
}

const Path &AStarPathFinder::getLastPath() {
    return _lastPath;
}
