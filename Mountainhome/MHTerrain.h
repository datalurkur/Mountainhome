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

#include "OctreeTileGrid.h"

class OctreeSceneManager;
class MaterialManager;
class TerrainPalette;

class MHTerrain {
public:
    MHTerrain(int width, int height, int depth, OctreeSceneManager *sceneManager, MaterialManager *materialManager):
        _width(width), _height(height), _depth(depth),
        _polyReduction(false), _autoUpdate(false),
        _materialManager(materialManager), _sceneManager(sceneManager) {
        _tilePalette = new TilePalette(materialManager);
    }

    virtual ~MHTerrain() {
        delete _tilePalette;
    }

    bool isOutOfBounds(Vector3 pos) { return isOutOfBounds(pos[0], pos[1], pos[2]); }
    bool isOutOfBounds(int x, int y, int z) {
        return (x < 0 || x >= _width  ||
                y < 0 || y >= _height ||
                z < 0 || z >= _depth);
    }

    virtual PaletteIndex getPaletteIndex(int x, int y, int z) = 0;
    virtual void setPaletteIndex(int x, int y, int z, PaletteIndex type) = 0;

	virtual int getSurfaceLevel(int x, int y) = 0;
    virtual void clear() = 0;

    virtual void save(const std::string &filename) = 0;
    virtual void load(const std::string &filename) = 0;

    virtual void populate() = 0;

    virtual int getWidth()  { return _width;  }
    virtual int getHeight() { return _height; }
    virtual int getDepth()  { return _depth;  }

    virtual void setPolyReduction(bool val) { _polyReduction = val; }
    virtual void setAutoUpdate   (bool val) { _autoUpdate    = val; }

    /* These use the TilePalette to do lookups and call the appropriate virtual setPaletteIndex/getPaletteIndex functions */
    bool        getTileProperty       (int x, int y, int z, TileProperty prop);
    char        getTileNumericProperty(int x, int y, int z, TileProperty prop);
    std::string getTileTextProperty   (int x, int y, int z, TileProperty prop);

    void setTileProperty(int x, int y, int z, TileProperty prop, PropertyType value);

    bool isTileEmpty(int x, int y, int z);
    void setTileEmpty(int x, int y, int z);

    TilePalette *getPalette() { return _tilePalette; }

protected:
    int _width, _height, _depth;

    bool _polyReduction;
    bool _autoUpdate;

    OctreeSceneManager *_sceneManager;
    MaterialManager *_materialManager;
    TilePalette *_tilePalette;
};

#endif
