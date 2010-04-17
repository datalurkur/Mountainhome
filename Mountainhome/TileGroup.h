/*
 *  TileGroup.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _TILEGROUP_H_
#define _TILEGROUP_H_
#include <Base/Vector.h>

class File;

/*! TileGroup represents an alternative way of storing the world. Rather than holding an
 *  individual tile object for each tile in the world, TileGroup attempts to group tiles
 *  up, thus drastically reducing the amount of memory needed to store large worlds. This
 *  particular implementation splits the world into a giant octree to aid in storing and
 *  looking up groups.
 * \note When splitting odd sized octants, this particular implementation always favors
 *  the lower half, granting it the larger half. This is VERY important to remember when
 *  determining which octant a location falls into or when dtermining the midway point of
 *  a tile group. */
class TileGroup {
public:
    /*! Use a short for now. */
    typedef short TileData;

    /*! Creates a new TileGroup.
     * \param pos  Represents the location of the lowest corner contained by this group.
     * \param dims The dimensions of this group.
     * \param type The type of tile this group contains.
     * \param parent This groups parent group (useful for pruning groups). */
    TileGroup(const Vector3 &position, const Vector3 &dimensions, TileData type, TileGroup* parent);

    /*! TileGroup's destructor */
    virtual ~TileGroup();

    /*! Gets the dimensions of this group. */
    Vector3 getDims();

    /*! Returns the maximum Z level at the given x/y location. */
    int getSurfaceLevel(const Vector2 &tileCoords);

    /*! Returns the tile type at the given location. */
    TileData getTile(const Vector3 &tileCoords);

    /*! Sets the tile type at the given location. */
    void setTile(const Vector3 &tileCoords, TileData type);

    /*! Deletes all of this group's children. */
    void clearChildren();

    /*! Writes this TileGroup's representative data and that of its octants to the file
     *  object passed as an argument, returning the total number of TileGroups written. */
    int write(File *tFile);

    /*! Takes position and dimensions of an octant somewhere in the TileGroup
     *  and attempts to add it, returning false if it fails. */
    void addOctant(const Vector3 &position, const Vector3 &dimensions, TileData type);

    /*! Debugging function to see the current state of the octree. */
    void examineOctree(int depth = 0);

    /*! Prints out some simple data on the group. */
    void printStats();

protected:
    /*! Searches downward through the octree for the lowest existing group that contains
     *  the given location. */
    TileGroup* getLowestGroup(const Vector3 &loc);

    /*! Returns the type of this group. */
    TileData getType();

    /*! Returns true if this TileGroup has no children. */
    bool isLeaf();

    /*! Returns true if this TileGroup represents exactly 1 tile. */
    bool isSmallest();

    /*! Because this implementation supports an odd sized world, not all TileGroups will
     *  actually be using all 8 octants. This functions determines if the current
     *  TileGroup supports the octant at the given index. */
    bool hasOctant(int index);

    /*! This examines all children and, if possible, will optimize memory usage by
     *  pruning children with no value or switching its base group to reduce the number of
     *  children it must support.
     *
     *  Pruning children: If all children of this TileGroup have the same type as this
     *  tile group, there is no reason for the children to exist as this TileGroup already
     *  adequately represents them.
     *
     *  Changing the base group: If this tile represents air, but has 7 sediment children,
     *  then space can be saved by switching this TileGroup over to sediment, deleting the
     *  7 sediment children, and creating a single child of type air. */
    bool optimizeGroup();

    /*! Sets the type of this group. */
    void setType(TileData type);

    /*! Finds the local children index based on the world space coords */
    int coordsToIndex(int x, int y, int z);

    /*! \see coordsToIndex(int, int, int) */
    int coordsToIndex(const Vector3 &coords);

    /*! Finds the position of the child at the given index. If that child does not exist,
     *  it will calculate what its position should be. */
    void indexToCoords(int index, Vector3 &coord);

    /*! Finds the dimensions of the child at the given index. If that child does not
     *  exist, it will calculate what its dimensions should be. */
    void indexToDims(int index, Vector3 &coord);

    /*! These are used to determine the dimensions of the different octants. If the group
     *  is even-sized, both the lower and upper version of these functions will return the
     *  same number. If the group is odd-sized, the lower set of octants will always be
     *  the larger.
     *
     *  Example: A group of size 9,9,9. The lower octants will be 5,5,5 and the upper will
     *  be 4,4,4. */
    inline int lowerWidth()  { return _dims.x - upperWidth();  }
    inline int lowerHeight() { return _dims.y - upperHeight(); }
    inline int lowerDepth()  { return _dims.z - upperDepth();  }

    inline int upperWidth()  { return _dims.x / 2; }
    inline int upperHeight() { return _dims.y / 2; }
    inline int upperDepth()  { return _dims.z / 2; }

    /*! These functions return the low value in this TileGroup. */
    inline int lowX() { return _pos.x; }
    inline int lowY() { return _pos.y; }
    inline int lowZ() { return _pos.z; }

    /*! These functions return the splitting point of this TileGroup. */
    inline int midX() { return lowX() + lowerWidth();  }
    inline int midY() { return lowY() + lowerHeight(); }
    inline int midZ() { return lowZ() + lowerDepth();  }

private:
    struct Chunk;
    friend struct Chunk;

    class TileGroupPool {
    public:
        TileGroupPool(int initialSize, TileGroup *parent);
        virtual ~TileGroupPool();

        TileGroup* getTileGroup(const Vector3 &position, const Vector3 &dimensions, TileData type, TileGroup* parent);

        void putTileGroup(TileGroup *&group);

        void printStats();

        TileGroup *getParent();

    protected:
        void createTileGroup();

        TileGroup *_parent;
        Chunk *_basePtr;
        Chunk *_freePtr;
        Chunk *_lastPtr;

        int _available;
        int _used;

    };

    /*! Creates an empty group using the given TileGroupPool to pull new children from. */
    TileGroup();

    /*! Sets the group's pool. */
    void setPool(TileGroupPool *pool);

    /*! Initializes the object with the appropriate values. */
    TileGroup* initialize(const Vector3 &position, const Vector3 &dimensions, TileData type, TileGroup* parent);

private:
    Vector3 _pos, _dims;     //!< This group's position and dimensions.
    TileData _type;          //!< Storage for tile information.

    TileGroupPool *_pool;    //!< The pool this group will pull children from.
    TileGroup* _children[8]; //!< The 8 possible children of this group.
    TileGroup* _parent;      //!< The parent of this TileGroup.

};

#endif
