#ifndef _LIQUIDSYSTEM_H_
#define _LIQUIDSYSTEM_H_

#include "LiquidGlob.h"
#include "LiquidFlow.h"

class LiquidSystem {
public:
    LiquidSystem(float msPerTick);
    ~LiquidSystem();

    void setup();
    void processLiquid(const Vector3 &node);
    void processVacuum(const Vector3 &node);
    void update(int elapsed);

private:
    bool _initialized;

    LiquidGlobList _globs;
    LiquidFlowList _flows;

    LiquidNodeSet _freeLiquids;
    LiquidNodeSet _vacuums;
    
    LiquidNodeSet _adjacency;
    
    float _msPerTick;
};

#endif
