/*
 *  OctreeTileGrid.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _OCTREETILEGRID_H_
#define _OCTREETILEGRID_H_
#include "TileGrid.h"

/*! OctreeTileGrid represents an alternative way of storing the world. Rather than holding an
 *  individual tile object for each tile in the world, OctreeTileGrid attempts to group tiles
 *  up, thus drastically reducing the amount of memory needed to store large worlds. This
 *  particular implementation splits the world into a giant octree to aid in storing and
 *  looking up groups.
 * \note When splitting odd sized octants, this particular implementation always favors
 *  the lower half, granting it the larger half. This is VERY important to remember when
 *  determining which octant a location falls into or when dtermining the midway point of
 *  a tile group. */
class OctreeTileGrid : public TileGrid {
public:
    /*! Creates a new OctreeTileGrid.
     * \param width  The width  of the grid.
     * \param height The height of the grid.
     * \param depth  The depth  of the grid.
     * \param pos    Represents the location of the lowest corner contained by this group.
     * \param type   The type of tile this group contains.
     * \param parent This groups parent group (useful for pruning groups). */
    OctreeTileGrid(int width, int height, int depth, const Vector3 &position,
        TileType type, OctreeTileGrid* parent);

    /*! OctreeTileGrid's destructor */
    virtual ~OctreeTileGrid();

#pragma mark TileGrid interface function declarations.
    /*! Returns the tile type at the given location. */
    virtual TileType getTile(int x, int y, int z);

    /*! Sets the tile type at the given location. */
    virtual void setTile(int x, int y, int z, TileType type);

    /*! Gets the maximum, full z level at the given x/y location. */
    virtual int getSurfaceLevel(int x, int y);

    /*! Saves the TileGrid to the given IOTarget. */
    virtual void save(IOTarget *target);

    /*! Loads the TileGrid from the given IOTarget. */
    virtual void load(IOTarget *target);

    /*! Deletes all of this group's children. */
    virtual void clear();

#pragma mark OctreeTileGrid specific functions.
    /*! Takes position and dimensions of an octant somewhere in the OctreeTileGrid
     *  and attempts to add it, returning false if it fails. */
    void addOctant(int width, int height, int depth, const Vector3 &position, TileType type);

    /*! Debugging function to see the current state of the octree. */
    void examineOctree(int depth = 0);

    /*! Prints out some simple data on the group. */
    void printStats();

protected:
    /*! Saves this specific group and all children, returning the number of groups saved. */
    int saveGroup(IOTarget *target);

    /*! Sets the tile type at the given location. */
    void setTile(const Vector3 &loc, TileType type);

    /*! Searches downward through the octree for the lowest existing group that contains
     *  the given location. */
    OctreeTileGrid* getLowestGroup(const Vector3 &loc);

    /*! Returns the type of this group. */
    TileType getType();
    TileType getType(TileType tData);
    TileType defaultType();

    /*! Returns true if this OctreeTileGrid has no children. */
    bool isLeaf();

    /*! Returns true if this OctreeTileGrid represents exactly 1 tile. */
    bool isSmallest();

    /*! Because this implementation supports an odd sized world, not all OctreeTileGrids will
     *  actually be using all 8 octants. This functions determines if the current
     *  OctreeTileGrid supports the octant at the given index. */
    bool hasOctant(int index);

    /*! This examines all children and, if possible, will optimize memory usage by
     *  pruning children with no value or switching its base group to reduce the number of
     *  children it must support.
     *
     *  Pruning children: If all children of this OctreeTileGrid have the same type as this
     *  tile group, there is no reason for the children to exist as this OctreeTileGrid already
     *  adequately represents them.
     *
     *  Changing the base group: If this tile represents air, but has 7 sediment children,
     *  then space can be saved by switching this OctreeTileGrid over to sediment, deleting the
     *  7 sediment children, and creating a single child of type air. */
    bool optimizeGroup();

    /*! Sets the type of this group. */
    void setType(TileType type);

    /*! Finds the local children index based on the world space coords */
    int coordsToIndex(int x, int y, int z);

    /*! \see coordsToIndex(int, int, int) */
    int coordsToIndex(const Vector3 &coords);

    /*! Finds the position of the child at the given index. If that child does not exist,
     *  it will calculate what its position should be. */
    void indexToCoords(int index, Vector3 &coord);

    /*! Finds the dimensions of the child at the given index. If that child does not
     *  exist, it will calculate what its dimensions should be. */
    void indexToDims(int index, int &width, int &height, int &depth);

    /*! These are used to determine the dimensions of the different octants. If the group
     *  is even-sized, both the lower and upper version of these functions will return the
     *  same number. If the group is odd-sized, the lower set of octants will always be
     *  the larger.
     *
     *  Example: A group of size 9,9,9. The lower octants will be 5,5,5 and the upper will
     *  be 4,4,4. */
    inline int lowerWidth()  { return _width  - upperWidth();  }
    inline int lowerHeight() { return _height - upperHeight(); }
    inline int lowerDepth()  { return _depth  - upperDepth();  }

    inline int upperWidth()  { return _width  / 2; }
    inline int upperHeight() { return _height / 2; }
    inline int upperDepth()  { return _depth  / 2; }

    /*! These functions return the low value in this OctreeTileGrid. */
    inline int lowX() { return _pos.x; }
    inline int lowY() { return _pos.y; }
    inline int lowZ() { return _pos.z; }

    /*! These functions return the splitting point of this OctreeTileGrid. */
    inline int midX() { return lowX() + lowerWidth();  }
    inline int midY() { return lowY() + lowerHeight(); }
    inline int midZ() { return lowZ() + lowerDepth();  }

private:
    struct Chunk;
    friend struct Chunk;

    class OctreeTileGridPool {
    public:
        OctreeTileGridPool(int initialSize, OctreeTileGrid *parent);
        virtual ~OctreeTileGridPool();

        OctreeTileGrid* getOctreeTileGrid(int width, int height, int depth,
            const Vector3 &position, TileType type, OctreeTileGrid* parent);

        void putOctreeTileGrid(OctreeTileGrid *&group);

        void printStats();

        OctreeTileGrid *getParent();

    protected:
        void createOctreeTileGrid();

        OctreeTileGrid *_parent;
        Chunk *_basePtr;
        Chunk *_freePtr;
        Chunk *_lastPtr;

        int _available;
        int _used;

    };

    /*! Creates an empty group using the given OctreeTileGridPool to pull new children from. */
    OctreeTileGrid();

    /*! Sets the group's pool. */
    void setPool(OctreeTileGridPool *pool);

    /*! Initializes the object with the appropriate values. */
    OctreeTileGrid* initialize(int width, int height, int depth, const Vector3 &position,
        TileType type, OctreeTileGrid* parent);

private:
    Vector3 _pos;   //!< This group's position and dimensions.
    TileType _type; //!< Storage for tile information.

    OctreeTileGridPool *_pool;    //!< The pool this group will pull children from.
    OctreeTileGrid* _children[8]; //!< The 8 possible children of this group.
    OctreeTileGrid* _parent;      //!< The parent of this OctreeTileGrid.

};

#endif
