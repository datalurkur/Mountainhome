/*
 * LiquidManager.h
 * Mountainhome
 *
 * Created by Andrew Jean on 6/23/11
 * Copyright 2011 Mountainhome Project. All rights reserved.
 */

#ifndef _LIQUIDMANAGER_H_
#define _LIQUIDMANAGER_H_

#include "Terrain.h"

class LiquidManager {
public:
    typedef std::set<Vector3> Vector3Set;

public:
    LiquidManager();
    ~LiquidManager();

    void liquidCreated(const Vector3 &coords);
    void vacuumCreated(const Vector3 &coords);

    bool update(int elapsed);
    void setup(Terrain *terrain);

    void getNewLiquids(Vector3Set &newLiquids);
    void clearNewLiquids();

private:
    bool _initialized;

    std::set<Vector3> _newLiquids;
    std::set<Vector3> _flowingLiquids;
    std::set<Vector3> _restingLiquids;

    std::set<Vector3> _vacuumNeighborhood;
    std::set<Vector3> _liquidNeighborhood;

    Terrain *_terrain;

private:
    static const int MSPerTick = 1000;
    static int MSSinceLastTick;
};

#endif
