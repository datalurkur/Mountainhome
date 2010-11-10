/*
 *  TileGrid.h
 *  Mountainhome
 *
 *  Created by loch on 7/9/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _TILEGRID_H_
#define _TILEGRID_H_
#include <Base/Vector.h>
class IOTarget;

// Tiles.

#define IS_LIQUID = 0x1;

struct Tile {
    char Type;
    unsigned int Parameters;

    Tile() {
        Type = NULL;
        Parameters = NULL;
    }

    Tile(char type, unsigned int params) {
        Type = type;
        Parameters = params;
    }

    void AddParameter(unsigned int params) {
        Parameters |= params;
    }

    void RemoveParameter(unsigned int params) {
        Parameters ^= params;
    }

    bool Equals(Tile other)
    {
        return this->Type == other.Type && this->Parameters == other.Parameters;
    }
};

typedef char TileType;

#define NEW_TILE(type) (*(new Tile(type, NULL)))
#define NEW_LIQUID(type) (*(new Tile(type, IS_LIQUID)))

class TileGrid {
public:
    static const TileType OutOfBounds = -1;

public:
    /*! Creates a new Tile grid of the given size. */
    TileGrid(int width, int height, int depth):
        _width(width), _height(height), _depth(depth) {}

    /*! D'tor */
    virtual ~TileGrid() {}

    /*! Gets the dimensions of this group. */
    Vector3 getDimensions() { return Vector3(_width, _height, _depth); } 

    /*! Gets the width of the grid. */
    int getWidth () { return _width ; }

    /*! Gets the height of the grid. */
    int getHeight() { return _height; }

    /*! Gets the depth of the grid. */
    int getDepth () { return _depth ; }

    /*! Sets the tile type at the specified location. */
    virtual void setTileType(int x, int y, int z, TileType type) = 0;

    /*! Sets the tile at the specified location. */
    virtual void setTile(int x, int y, int z, Tile type) = 0;

    /*! Gets the tile type at the specified location. */
    virtual TileType getTileType(int x, int y, int z) = 0;

    /*! Gets the tile at the specified location. */
    virtual Tile getTile(int x, int y, int z) = 0;

    /*! Gets the maximum, full z level at the given x/y location. */
    virtual int getSurfaceLevel(int x, int y) = 0;

    /*! Saves the TileGrid to the given IOTarget. */
    virtual void save(IOTarget *target) = 0;

    /*! Loads the TileGrid from the given IOTarget. */
    virtual void load(IOTarget *target) = 0;

    /*! Clears out the entire grid. */
    virtual void clear() = 0;

protected:
    int _width, _height, _depth;

};

#endif
