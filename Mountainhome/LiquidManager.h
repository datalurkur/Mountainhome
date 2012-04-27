/*
 * LiquidManager.h
 * Mountainhome
 *
 * Created by Andrew Jean on 6/23/11
 * Copyright 2011 Mountainhome Project. All rights reserved.
 */

#ifndef _LIQUIDMANAGER_H_
#define _LIQUIDMANAGER_H_

#include <vector>
#include <list>
#include <map>
#include "Terrain.h"

typedef std::pair<VALUE,int> Liquid;
typedef std::map<Vector3, std::pair<Vector3, int> > OutflowMap;
typedef std::map<Vector3, Vector3> InflowMap;

class LiquidManager {
public:
    LiquidManager();
    ~LiquidManager();

    void addLiquidType(VALUE typeValue, int flowRate);
    bool isLiquid(const Voxel *type);
    int getFlowRate(const Voxel *type);

    void setFlow(Vector3 source, Vector3 dest, int offset);
    void rerouteFlowDest(Vector3 source, Vector3 dest, int offset, Terrain *terrain);
    void rerouteFlowSource(Vector3 source, Vector3 dest, int offset, Terrain *terrain);

    void processLiquid(Vector3 coords, int time, Terrain *terrain);
    void processVacuum(Vector3 coords, int time, Terrain *terrain);

    void deleteInflow(Vector3 coords);
    void deleteOutflow(Vector3 coords);

    void updateFlows(int elapsed);
    bool hasNextFlow();
    int getNextFlow(Vector3 &source, Vector3 &dest);

private:
    std::vector<Liquid> _liquidTypes;
    std::list<Vector3> _readyToFlow;

    InflowMap _inflows;
    OutflowMap _outflows;

    void printOutflows();
    void printInflows();
};

#endif
