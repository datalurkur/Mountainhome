#ifndef _LIQUIDFLOW_H_
#define _LIQUIDFLOW_H_

#include "LiquidGlob.h"
#include <queue>

class LiquidSource {
public:
    LiquidSource(const LiquidSource &other);
    LiquidSource(const Vector3 &point, LiquidGlob *glob);
    
    const Vector3 &point();
    LiquidGlob *glob();
    
    bool operator<(const LiquidSource &other) const;

protected:
    Vector3 _point;
    LiquidGlob *_glob;
};

typedef std::list<LiquidSource> LiquidSourceList;
typedef std::priority_queue<LiquidSource> SortedLiquidSources;

class LiquidFlow: public LiquidSource {
public:
    LiquidFlow(const LiquidFlow &other);
    LiquidFlow(const Vector3 &point, LiquidGlob *glob);

    void addSource(const Vector3 &point, LiquidGlob *glob);
    void addSources(const LiquidSourceList &sources);
    void getSources(LiquidSourceList &sources);
    void getSortedSources(SortedLiquidSources &sources);

private:
    LiquidSourceList _sources;
};

typedef std::list<LiquidFlow> LiquidFlowList;

#endif
