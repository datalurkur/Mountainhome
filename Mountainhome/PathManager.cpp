#include "PathManager.h"

// ==================
// PathEdge Functions
// ==================

PathEdge::PathEdge(PathNodeCluster *cA, PathNodeCluster *cB, Vector3 nA, Vector3 nB, PathWeight w):
    clusterA(cA), clusterB(cB), nodeA(nA), nodeB(nB), weight(w) {
    clusterA->addEdge(this);
    clusterB->addEdge(this);
}

PathEdge::~PathEdge() {
    clusterA->removeEdge(this);
    clusterB->removeEdge(this);
}

// =========================
// PathNodeCluster Functions
// =========================

PathNodeCluster::PathNodeCluster(Vector3 min, Vector3 max, ClusterType type):
    _min(min), _max(max), _type(type) {
}

PathNodeCluster::~PathNodeCluster() { }

const EdgeList &PathNodeCluster::getEdges() {
    return _edges;
}

Vector3 PathNodeCluster::getMin() { return _min; }
void PathNodeCluster::setMin(const Vector3 &min) { _min = min; }

Vector3 PathNodeCluster::getMax() { return _max; }
void PathNodeCluster::setMax(const Vector3 &max) { _max = max; }

Vector3 PathNodeCluster::getCenter() { return _min + ((_max - _min) / 2.0); }

ClusterType PathNodeCluster::getType() { return _type; }

bool PathNodeCluster::contains(Vector3 point) {
    if(point[0] < _min[0] || point[0] > _max[0] ||
       point[1] < _min[1] || point[1] > _max[1] ||
       point[2] < _min[2] || point[2] > _max[2]) { return false; }
    else                                         { return true; }
}

bool PathNodeCluster::addEdge(PathEdge *edge) {
    EdgeIterator itr = _edges.begin();
    for(; itr != _edges.end(); itr++) {
        if((*itr) == edge) { return false; }
    }
    _edges.push_back(edge);
    return true;
}

bool PathNodeCluster::removeEdge(PathEdge *edge) {
    EdgeIterator itr = _edges.begin();
    for(; itr != _edges.end(); itr++) {
        if((*itr) == edge) {
            _edges.remove(edge);
            return true;
        }
    }
    return false;
}

PathEdge *PathNodeCluster::findEdge(Vector3 pointA, Vector3 pointB) {
    EdgeIterator itr = _edges.begin();
    for(; itr != _edges.end(); itr++) {
        if(((*itr)->nodeA == pointA && (*itr)->nodeB == pointB) ||
           ((*itr)->nodeA == pointB && (*itr)->nodeB == pointA)) {
            return (*itr);
        }
    }
    return NULL;
}

// =====================
// PathManager Functions
// =====================

PathManager::PathManager(Vector3 dimensions):
    _dimensions(dimensions)
{
    _clusters = new ClusterList(_dimensions[0] * _dimensions[1] * _dimensions[2]);
    PathNodeCluster *defaultCluster = new PathNodeCluster(Vector3(0,0,0), _dimensions - Vector3(1,1,1), CLOSED);
    for(int x=0; x<_dimensions.x; x++) {
        for(int y=0; y<_dimensions.y; y++) {
            for(int z=0; z<_dimensions.z; z++) {
                setCluster(x,y,z,defaultCluster);
            }
        }
    }
}

PathManager::~PathManager() {
    clear_list(_edges);
    for(int i=0; i<_clusters->size(); i++) {
        PathNodeCluster *thisCluster = (*_clusters)[i];
        if(thisCluster != NULL) {
            Vector3 min = thisCluster->getMin(),
                    max = thisCluster->getMax();
            for(int x=min.x; x<=max.x; x++) {
                for(int y=min.y; y<=max.y; y++) {
                    for(int z=min.z; z<=max.z; z++) {
                        setCluster(x,y,z,NULL);
                    }
                }
            }
            delete thisCluster;
        }
    }
    delete _clusters;
}

// Returns true if a change was made that requires nodes to be regrouped
void PathManager::setNodeType(int x, int y, int z, ClusterType type) {
    PathNodeCluster *thisCluster = getCluster(x,y,z);

    // If the types are the same, there's no work to be done, bail out
    ClusterType oldType = thisCluster->getType();
    if(oldType == type) { return; }

    // Get the dimensions of this cluster
    Vector3 min = thisCluster->getMin(),
            max = thisCluster->getMax();

    // Delete the old edges for this cluster
    clearClusterEdges(thisCluster);

    // Create new clusters in place of this one
    int i, j, k;
    for(i=min.x; i<=max.x; i++) {
        for(j=min.y; j<=max.y; j++) {
            for(k=min.z; k<=max.z; k++) {
                Vector3 thisPoint(i,j,k);
                ClusterType thisType = oldType;
                if(i == x && j == y && k ==z ) { thisType = type; }
                setCluster(i,j,k, new PathNodeCluster(thisPoint, thisPoint, thisType));
            }
        }
    }

    delete thisCluster;

    // Fill in edges for the new clusters
    for(i=min.x; i<=max.x; i++) {
        for(j=min.y; j<=max.y; j++) {
            for(k=min.z; k<=max.z; k++) {
                updateEdgesAt(i,j,k);
            }
        }
    }

    // Regroup the nodes affected
    regroupClusters(min, max);
}

// Checks neighbors and updates edges for the cluster at the given coordinates
void PathManager::updateEdgesAt(int x, int y, int z) {
    PathNodeCluster *thisCluster = getCluster(x,y,z);

    // A non-pathable cluster should contain no edges
    if(thisCluster->getType() != PATHABLE) {
        clearClusterEdges(thisCluster);
        return;
    }

    // Loop over all of the immediate neighbors of this node
    for(int i=-1; i<=1; i++) {
        int local_x = x+i;
        if(local_x < 0 || local_x >= _dimensions.x) { continue; }

        for(int j=-1; j<=1; j++) {
            int local_y = y+j;
            if(local_y < 0 || local_y >= _dimensions.y) { continue; }
            if(i == 0 && j == 0) { continue; } // This node need not be connected to itself

            // Check to see if this edge can exist
            // Determine if we're trying to traverse upwards, downwards, or horizontally
            int local_z = z;
            if(getType(local_x, local_y, local_z) == CLOSED) {
                if(++local_z == _dimensions.z) { continue; }
            } else if(getType(local_x, local_y, local_z) == OPEN) {
                local_z--;
            }

            // Skip this edge if these nodes are in the same cluster
            PathNodeCluster *thatCluster = getCluster(local_x, local_y, local_z);
            if(thisCluster == thatCluster) { continue; }

            if(thatCluster->getType() == PATHABLE) {
                // Add an edge here if it does not already exist
                createEdge(Vector3(x,y,z), Vector3(local_x,local_y,local_z));
            } else {
                // Remove this edge if it exists
                destroyEdge(Vector3(x,y,z), Vector3(local_x,local_y,local_z));
            }
        }
    }
}

bool PathManager::destroyEdge(Vector3 pointA, Vector3 pointB) {
    PathNodeCluster* clusterA = getCluster(pointA.x, pointA.y, pointA.z);
    PathEdge *edge = clusterA->findEdge(pointA, pointB);
    if(edge != NULL) {
        _edges.remove(edge);
        delete edge;
        return true;
    } else {
        return false;
    }
}

bool PathManager::createEdge(Vector3 pointA, Vector3 pointB) {
    PathNodeCluster *clusterA = getCluster(pointA.x, pointA.y, pointA.z),
                    *clusterB = getCluster(pointB.x, pointB.y, pointB.z);
    if(clusterA->findEdge(pointA, pointB) == NULL) {
        PathEdge *newEdge = new PathEdge(clusterA, clusterB, pointA, pointB, distance(pointA, pointB));
        _edges.push_back(newEdge);
        return true;
    } else {
        return false;
    }
}

void PathManager::clearClusterEdges(PathNodeCluster *cluster) {
    EdgeList edges = cluster->getEdges();
    ConstEdgeIterator itr = edges.begin();
    for(; itr != edges.end(); itr++) {
        _edges.remove((*itr));
        delete (*itr);
    }
}

// Accepts two vectors as boundaries for clusters that need grouping;
//  the clusters contained within and the immediately adjacent clusters will be examined,
//  expanding the are to be grouped if clusters are grouped with clusters outside the area
void PathManager::regroupClusters(Vector3 min, Vector3 max) {
    bool done = false;
    while(!done) {
        done = true;

        std::list<PathNodeCluster*> visitedClusters;
        for(int x = min.x; x <= max.x; x++) {
            for(int y = min.y; y <= max.y; y++) {
                for(int z = min.z; z <= max.z; z++) {
                    PathNodeCluster *thisCluster = getCluster(x, y, z);

                    // Skip any clusters that have already been collapsed as much as they can
                    std::list<PathNodeCluster*>::iterator itr = visitedClusters.begin();
                    for(; itr != visitedClusters.end(); itr++) { if(thisCluster == (*itr)) { break; } }

                    if(itr == visitedClusters.end()) {
                        if(growCluster(thisCluster, visitedClusters)) {
                            // Cluster was collapsed update the corners to accurately reflect the new size
                            if(min < thisCluster->getMin()) { min = thisCluster->getMin(); }
                            if(max > thisCluster->getMax()) { max = thisCluster->getMax(); }
                        }
                    }
                }
            }
        }
    }
}

bool PathManager::growCluster(PathNodeCluster *thisCluster, std::list<PathNodeCluster*> &visitedClusters) {
    bool done = false, collapsed = false;
    while(!done) {
        done = true;

        Vector3 thisMin = thisCluster->getMin(),
                thisMax = thisCluster->getMax();

        // Check each connected cluster to see if these clusters can collapse
        EdgeList edges = thisCluster->getEdges();
        EdgeIterator edgeItr = edges.begin();
        for(; edgeItr != edges.end(); edgeItr++) {
            PathNodeCluster *connectedCluster = ((*edgeItr)->clusterA == thisCluster) ? (*edgeItr)->clusterB : (*edgeItr)->clusterA;
            Vector3 connectedMin = connectedCluster->getMin(),
                    connectedMax = connectedCluster->getMax();

            // Determine which axes have shared positions and dimensions
            char setAxes = 0;
            Vector3 newMin, newMax;
            if(thisMin.x == connectedMin.x && thisMax.x == connectedMax.x) {
                setAxes |= 0x1;
                newMin.x = thisMin.x;
                newMax.x = thisMax.x;
            }
            if(thisMin.y == connectedMin.y && thisMax.y == connectedMax.y) {
                setAxes |= 0x2;
                newMin.y = thisMin.y;
                newMax.y = thisMax.y;
            }
            if(thisMin.z == connectedMin.z && thisMax.z == connectedMax.z) {
                setAxes |= 0x4;
                newMin.z = thisMin.z;
                newMax.z = thisMax.z;
            }

            if(setAxes == 0x3) {
                // X/Y plane is shared, collapse is possible
                newMin.z = Math::Min(thisMin.z, connectedMin.z);
                newMax.z = Math::Max(thisMax.z, connectedMax.z);
            } else if(setAxes == 0x5) {
                // X/Z plane is shared, collapse is possible
                newMin.y = Math::Min(thisMin.y, connectedMin.y);
                newMax.y = Math::Max(thisMax.y, connectedMax.y);
            } else if(setAxes == 0x6) {
                // Y/Z plane is shared, collapse is possible
                newMin.x = Math::Min(thisMin.x, connectedMin.x);
                newMax.x = Math::Max(thisMax.x, connectedMax.x);
            } else {
                // Collapse not possible
                continue;
            }

            done = false;
            collapsed = true;

            // Remove the connected cluster from the visited list (if it exists there)
            visitedClusters.remove(connectedCluster);

            // Collapse the two clusters
            collapseClusters(thisCluster, connectedCluster);

            // Expand this cluster to encapsulate the connected cluster
            thisCluster->setMin(newMin);
            thisCluster->setMax(newMax);

            // Break out, start the loop over again with the new edges
            break;
        }
    }

    return collapsed;
}

void PathManager::collapseClusters(PathNodeCluster *host, PathNodeCluster *guest) {
    // Reroute and add the edges of the connected cluster to this one, removing internal edges as necessary
    EdgeList guestEdges = guest->getEdges();
    ConstEdgeIterator itr = guestEdges.begin();
    for(; itr != guestEdges.end(); itr++) {
        if((*itr)->clusterA == host || (*itr)->clusterB == host) {
            _edges.remove(*itr);
            delete (*itr); // No need to maintain connections internal to these two clusters
        } else {
            if((*itr)->clusterA == guest) {
                (*itr)->clusterA = host;
            } else if((*itr)->clusterB == guest) {
                (*itr)->clusterB = host;
            }
            host->addEdge(*itr);
        }
    }

    // Set the old cluster's position to point to the new cluster
    Vector3 min = guest->getMin(),
            max = guest->getMax();
    for(int x=min.x; x<=max.x; x++) {
        for(int y=min.y; y<=max.y; y++) {
            for(int z=min.z; z<=max.z; z++) {
                setCluster(x,y,z,host);
            }
        }
    }

    // Delete the old cluster
    delete guest;
}

Vector3 PathManager::getDimensions() const {
    return _dimensions;
}
const EdgeList &PathManager::getEdges() const {
    return _edges;
}
ClusterList *PathManager::getClusters() const {
    return _clusters;
}
inline int PathManager::getIndex(int x, int y, int z) {
    return ((z*_dimensions[0]*_dimensions[1])+(y*_dimensions[0])+x);
}
inline PathNodeCluster *PathManager::getCluster(int x, int y, int z) {
    return (*_clusters)[getIndex(x, y, z)];
}
inline ClusterType PathManager::getType(int x, int y, int z) {
    return getCluster(x,y,z)->getType();
}
inline void PathManager::setCluster(int x, int y, int z, PathNodeCluster *cluster) {
    (*_clusters)[getIndex(x,y,z)] = cluster;
}
inline int PathManager::distance(Vector3 start, Vector3 end) {
    return (end-start).length();
}
