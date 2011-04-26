#ifndef _PATHMANAGER_H_
#define _PATHMANAGER_H_

#define PATH_NOT_FOUND -1

typedef char NodeType;
enum {
    PATHABLE = 0,
    CLOSED,
    OPEN,
    NODE_TYPES
};

typedef char PathWeight;
#define CARDINAL_WEIGHT 10
#define DIAGONAL_WEIGHT 14
#define CORNER_WEIGHT   17

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
    bool removeEdge(PathNode *otherNode);
    const EdgeList &getEdges();

    Vector3 getLowerCorner();
    Vector3 getUpperCorner();
    Vector3 getCenter();

    bool contains(Vector3 point);

private:
    NodeType _type;
    Vector3 _lowerCorner,
            _upperCorner;
    EdgeList _edges;
};

typedef std::vector<PathNode*> NodeList;
typedef NodeList::iterator NodeIterator;

typedef std::stack<Vector3> Path;

#define NODE_INDEX(x,y,z) ((z * _dimensions[0] * _dimensions[1]) + (y * _dimensions[0]) + x)
#define NODE(x,y,z) _nodes[NODE_INDEX(x,y,z)]

class PathManager {
public:
    PathManager(Vector3 dimensions);
    ~PathManager();

    bool setNodeType(int x, int y, int z, NodeType type);

    virtual int getPath(Vector3 start, Vector3 end, Path &path) = 0;

private:
    void addEdgesFor(int x, int y, int z);

    void regroupNodes(Vector3 lowerCorner, Vector3 upperCorner);

    void createNode(Vector3 lowerCorner, Vector3 upperCorner, NodeType type);
    void deleteNode(PathNode *node);

private:
    Vector3 _dimensions;
    NodeList *_nodes;
};

#endif
