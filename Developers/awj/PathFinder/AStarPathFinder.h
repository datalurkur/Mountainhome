#ifndef _ASTARPATHFINDER_H_
#define _ASTARPATHFINDER_H_

#include "PathManager.h"

typedef std::list<AStarNode*> AStarList;

class AStarNode {
public:
    AStarNode(AStarNode *parent, PathNode *node, int gCost, int hCost);

    int fCost() const;
    int hCost() const;

    void setGCost(int value);
    void setParent(AStartNode *parent);

    PathNode *getPathNode();

    bool operator<(const AStarNode &other);

private:
    AStarNode *_parent;
    PathNode *_pathNode;
    int _gCost, _hCost;
};

class AStarPathFinder : public PathManager {
public:
    virtual int getPath(Vector3 start, Vector3 end, Path &path);

private:
    void updateOpenList(AStarNode *currentNode, AStarList &openList);

    int fillPath(Vector3 start, Vector3 end, AStarNode *currentNode, Path &path);

    int distance(Vector3 start, Vector3 end);
};

#endif
