#ifndef _PATHMANAGER_H_
#define _PATHMANAGER_H_

#include <Base/Vector.h>
#include <utility>
#include <stack>

// ===========================
// Basic Structure Definitions
// ===========================
struct PathEdge;
class PathNodeCluster;
class PathManager;

// ========
// Subtypes
// ========
typedef char ClusterType;
enum {
    PATHABLE = 0,
    CLOSED,
    OPEN,
    NODE_TYPES
};

typedef float PathWeight;

// ========
// PathEdge
// ========
struct PathEdge {
    PathEdge(PathNodeCluster *cA, PathNodeCluster *cB, Vector3 nA, Vector3 nB, PathWeight weight);
    ~PathEdge();

    PathNodeCluster *clusterA, *clusterB;
    Vector3 nodeA, nodeB;
    PathWeight weight;
};

typedef std::list<PathEdge*> EdgeList;
typedef EdgeList::iterator EdgeIterator;
typedef EdgeList::const_iterator ConstEdgeIterator;

// ===============
// PathNodeCluster
// ===============
class PathNodeCluster {
public:
    PathNodeCluster(Vector3 min, Vector3 max, ClusterType type);
    ~PathNodeCluster();

    const EdgeList &getEdges();

    Vector3 getMin();
    void setMin(const Vector3 &min);

    Vector3 getMax();
    void setMax(const Vector3 &max);

    Vector3 getCenter();

    ClusterType getType();

    bool contains(Vector3 point);

    bool addEdge(PathEdge *edge);
    bool removeEdge(PathEdge *edge);
    PathEdge *findEdge(Vector3 pointA, Vector3 pointB);

private:
    ClusterType _type;
    EdgeList _edges;
    Vector3 _min, _max;
};

typedef std::vector<PathNodeCluster*> ClusterList;
typedef std::vector<Vector3> Path;
#define PATH_NOT_FOUND -1

// ===========
// PathManager
// ===========
class PathManager {
public:
    PathManager(Vector3 dimensions);
    virtual ~PathManager();

    // Functions used by things that query pathfinding state
    virtual int getPath(Vector3 start, Vector3 end, Path &path) = 0;

    // Functions used by things that set up pathfinding state
    void setNodeType(int x, int y, int z, ClusterType type);

    Vector3 getDimensions() const;
    const EdgeList &getEdges() const;
    ClusterList *getClusters() const;

// DEBUGGING TOOL
    virtual const Path &getLastPath() = 0;

protected:
    // Inlines used to get members of the cluster list
    int getIndex(int x, int y, int z);
    PathNodeCluster *getCluster(int x, int y, int z);
    ClusterType getType(int x, int y, int z);

    int distance(Vector3 start, Vector3 end);
    
private:
    // Inline used to set a member of the cluster list
    void setCluster(int x, int y, int z, PathNodeCluster *node);

    // Checks neighbors and updates appropriate edges for the coordinates given
    void updateEdgesAt(int x, int y, int z);

    // Functions used to maintain edges
    bool createEdge(Vector3 pointA, Vector3 pointB);
    bool destroyEdge(Vector3 pointA, Vector3 pointB);

    void clearClusterEdges(PathNodeCluster *node);

private:
    Vector3 _dimensions;
    EdgeList _edges;
    ClusterList *_clusters;
};

#endif
