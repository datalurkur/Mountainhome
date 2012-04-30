#ifndef _LIQUIDGLOB_H_
#define _LIQUIDGLOB_H_

#include <Base/Vector.h>
#include <Base/AABB.h>

typedef std::set<Vector3> LiquidNodeSet;

class LiquidGlob {
public:
    LiquidGlob(const Vector3 &node, float surfaceLevel);
    LiquidGlob(const LiquidNodeSet &startingSet);
    ~LiquidGlob();

    void addNode(const Vector3 &node);

    // Used for collecting a nodeset into globs
    void consolidate(LiquidNodeSet &remaining, const LiquidNodeSet &adjacency, const Vector3 &startingNode);

    // Data getters
    float getSurfaceLevel();

    // Used for determining the relationship between a node and this glob
    bool findSource(const Vector3 &node, const LiquidNodeSet &adjacency, Vector3 &abutting);

private:
    bool couldAbut(const Vector3 &node);

    bool couldContain(const Vector3 &node);
    bool contains(const Vector3 &node);

private:
    LiquidNodeSet _nodes;

    int _surfaceNodes;
    int _surfaceZ;
    float _surfaceLevel;

    AABB3 _bounds;
};

typedef std::list<LiquidGlob*> LiquidGlobList;

#endif
