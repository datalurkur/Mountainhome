    #include "LiquidGlob.h"

LiquidGlob::LiquidGlob(const Vector3 &node, float surfaceLevel): _surfaceNodes(0), _surfaceZ(-1), _surfaceLevel(surfaceLevel) {
    addNode(node);
}

LiquidGlob::LiquidGlob(const LiquidNodeSet &startingSet): _surfaceNodes(0), _surfaceZ(-1), _surfaceLevel(1.0) {
    LiquidNodeSet::const_iterator itr;
    for(itr = startingSet.begin(); itr != startingSet.end(); itr++) {
        addNode(*itr);
    }
}

LiquidGlob::~LiquidGlob() {
}

void LiquidGlob::addNode(const Vector3 &node) {
    ASSERT(_nodes.find(node) == _nodes.end());

    // Adjust surface statistics
    if(node.z > _surfaceZ) {
        _surfaceZ = node.z;
        _surfaceNodes = 1;
    } else if(node.z == _surfaceZ) {
        _surfaceNodes++;
    }

    // Reshape the AABB
    if(_nodes.empty()) {
        _bounds.clear();
        _bounds.setCenter(node);
    } else {
        _bounds.encompass(node);
    }

    // Insert the node
    _nodes.insert(node);
}

void LiquidGlob::consolidate(LiquidNodeSet &remaining, const LiquidNodeSet &adjacency, const Vector3 &startingNode) {
    LiquidNodeSet closedGroup, openGroup;
    LiquidNodeSet::iterator itr, node;
    LiquidNodeSet::const_iterator adjacentItr;
    Vector3 adjacentNode;

    Info("Consolidating liquid glob from " << remaining.size() << " nodes.");

    // Find the starting node and begin there
    node = _nodes.find(startingNode);
    _nodes.erase(node);
    openGroup.insert(*node);

    // Take a node in the open set, add it to the closed set, and add all of adjacent nodes that are pertinent to the open set
    while(!openGroup.empty()) {
        node = openGroup.begin();
        openGroup.erase(node);

        //Info("Adding node " << (*node) << " to glob");
        closedGroup.insert(*node);

        for(adjacentItr = adjacency.begin(); adjacentItr != adjacency.end(); adjacentItr++) {
            adjacentNode = Vector3((*adjacentItr) + (*node));
            itr = _nodes.find(adjacentNode);
            if(itr != _nodes.end()) {
                _nodes.erase(itr);
                openGroup.insert(*itr);
            }
        }
    }

    remaining = _nodes;
    _nodes.clear();
    // Reset the surface Z
    _surfaceZ = -1;

    // Move the closed group into this group's node set
    for(itr = closedGroup.begin(); itr != closedGroup.end(); itr++) {
        addNode(*itr);
    }

    Info("Resulting glob has z-level " << _surfaceZ << " with " << _surfaceNodes << " surface nodes and AABB " << _bounds);
}

float LiquidGlob::getSurfaceLevel() { return _surfaceZ + _surfaceLevel; }
//float LiquidGlob::getSurfaceVolume() { return _surfaceNodes * _surfaceLevel; }

// Checks to see if some part of this glob is adjacent to a given node, returning the first node in the glob it finds that is touching
bool LiquidGlob::findSource(const Vector3 &node, const LiquidNodeSet &adjacency, Vector3 &outlet) {
    LiquidNodeSet::const_iterator adjacentItr;

    if(!couldAbut(node)) { return false; }

    for(adjacentItr = adjacency.begin(); adjacentItr != adjacency.end(); adjacentItr++) {
        if(_nodes.find((*adjacentItr) + node) != _nodes.end()) { return true; }
    }

    return false;
}

bool LiquidGlob::contains(const Vector3 &node) {
    return (_nodes.find(node) != _nodes.end());
}

bool LiquidGlob::couldContain(const Vector3 &node) {
    return _bounds.contains(node, false);
}

bool LiquidGlob::couldAbut(const Vector3 &node) {
    return _bounds.contains(node, true);
}
