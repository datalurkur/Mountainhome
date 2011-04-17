/*
 *  MHTerrain.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHTERRAIN_H_
#define _MHTERRAIN_H_
#include <Base/Vector.h>
#include <vector>

#include "TilePalette.h"

class OctreeSceneManager;
class TerrainPalette;

class MHTerrain {
public:
    MHTerrain(int width, int height, int depth, OctreeSceneManager *sceneManager);
    virtual ~MHTerrain();

    bool isOutOfBounds(Vector3 pos);
    bool isOutOfBounds(int x, int y, int z);

    virtual PaletteIndex getPaletteIndex(int x, int y, int z) = 0;
    virtual void setPaletteIndex(int x, int y, int z, PaletteIndex type) = 0;

	virtual int getSurfaceLevel(int x, int y) = 0;
    virtual int getEmptyRanges(int x, int y, std::vector<std::pair<int,int> > &ranges) = 0;
    virtual int getFilledRanges(int x, int y, std::vector<std::pair<int,int> > &ranges) = 0;

    virtual void clear() = 0;

    virtual void save(const std::string &filename) = 0;
    virtual void load(const std::string &filename) = 0;

    virtual void populate() = 0;

    virtual int getWidth();
    virtual int getHeight();
    virtual int getDepth();

    virtual void setPolyReduction(bool val);
    virtual void setAutoUpdate   (bool val);

    TilePalette *getPalette();

    void setTile(int x, int y, int z, const Tile &tile);
    void setTileIndex(int x, int y, int z, PaletteIndex index);
    void setTileParameter(int x, int y, int z, ParameterID id, const ParameterData &value);
    const ParameterData &getTileParameter(int x, int y, int z, ParameterID id);
    const Tile &getTile(int x, int y, int z);

    bool isTileEmpty(int x, int y, int z);
    void setTileEmpty(int x, int y, int z);

protected:
    int _width, _height, _depth;

    bool _polyReduction;
    bool _autoUpdate;

    OctreeSceneManager *_sceneManager;
    TilePalette *_tilePalette;
};

#endif
