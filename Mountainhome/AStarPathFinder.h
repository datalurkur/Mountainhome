#ifndef _ASTARPATHFINDER_H_
#define _ASTARPATHFINDER_H_

#include "PathManager.h"

class AStarNode {
public:
    AStarNode(PathNodeCluster *cluster, Vector3 prePosition, Vector3 postPosition, AStarNode *parent, int gCost, int hCost);

    int fCost() const;
    int hCost() const;

    void setGCost(int value);
    void setParent(AStarNode *parent);
    void setPrePosition(Vector3 position);

    PathNodeCluster *getCluster() const;
    AStarNode *getParent() const;
    Vector3 getPrePosition() const;
    Vector3 getPostPosition() const;

private:
    AStarNode *_parent;
    PathNodeCluster *_cluster;
    Vector3 _prePosition, _postPosition;
    int _gCost, _hCost;
};

extern bool compareNodes(AStarNode *first, AStarNode *second);

typedef std::list<AStarNode*> AStarList;

class AStarPathFinder : public PathManager {
public:
    AStarPathFinder(Vector3 dimensions);

    virtual int getPath(Vector3 start, Vector3 end, Path &path);

// DEBUGGING TOOL
    virtual const Path &getLastPath();

private:
    AStarList _openList;
    AStarList _closedList;

// DEBUGGING TOOL
    Path _lastPath;
};

#endif
