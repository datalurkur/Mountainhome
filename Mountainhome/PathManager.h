#ifndef _PATHMANAGER_H_
#define _PATHMANAGER_H_

#include <Base/Vector.h>
#include <utility>
#include <stack>

#define PATH_NOT_FOUND -1

typedef char NodeType;
enum {
    PATHABLE = 0,
    CLOSED,
    OPEN,
    NODE_TYPES
};

typedef char PathWeight;
#define CARDINAL_WEIGHT 1
#define DIAGONAL_WEIGHT 1.4
#define CORNER_WEIGHT   1.7

class PathNode;
typedef std::pair<PathNode*, PathWeight> PathEdge;

typedef std::list<PathEdge> EdgeList;
typedef EdgeList::iterator EdgeIterator;
typedef EdgeList::const_iterator ConstEdgeIterator;

class PathNode {
public:
    PathNode(Vector3 lowerCorner, Vector3 upperCorner, NodeType type);
    ~PathNode();

    void setType(NodeType type);
    NodeType getType();

    bool addEdge(PathNode *otherNode, PathWeight weight);
    void addEdges(const EdgeList &edges);

    bool removeEdge(PathNode *otherNode);

    const EdgeList &getEdges();
    bool hasEdge(PathNode *node);

    Vector3 getLowerCorner();
    Vector3 getUpperCorner();
    Vector3 getCenter();

    void setLowerCorner(const Vector3 &corner);
    void setUpperCorner(const Vector3 &corner);

    bool contains(Vector3 point);

private:
    NodeType _type;
    Vector3 _lowerCorner,
            _upperCorner;
    EdgeList _edges;
};

typedef std::vector<PathNode*> NodeList;
typedef NodeList::iterator NodeIterator;

typedef std::vector<Vector3> Path;

class PathManager {
public:
    PathManager(Vector3 dimensions);
    virtual ~PathManager();

    Vector3 getDimensions() const;
    NodeList *getNodes() const;

    void setNodeType(int x, int y, int z, NodeType type);

    virtual int getPath(Vector3 start, Vector3 end, Path &path) = 0;

// DEBUGGING TOOLS
    bool verifyEdges();
    virtual const Path &getLastPath() = 0;

protected:
    int getIndex(int x, int y, int z);
    PathNode *getNode(int x, int y, int z);
    
private:
    void setNode(int x, int y, int z, PathNode *node);

    void addEdgesFor(int x, int y, int z);

    void regroupNodes(Vector3 lowerCorner, Vector3 upperCorner);
    bool growNode(PathNode *thisNode, std::list<PathNode*> &visitedNodes);
    void collapseNodes(PathNode *host, PathNode *guest);

    void createNode(Vector3 lowerCorner, Vector3 upperCorner, NodeType type);
    void deleteNode(PathNode *node);

private:
    Vector3 _dimensions;
    NodeList *_nodes;
};

#endif
