/*
 *  OctreeVoxelGrid.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

///\todo XXXBMW: Needs cleanup!!!!

#ifndef _OCTREETILEGRID_H_
#define _OCTREETILEGRID_H_
#include "VoxelGrid.h"

/*! OctreeVoxelGrid represents an alternative way of storing the world. Rather than holding an
 *  individual voxel object for each voxel in the world, OctreeVoxelGrid attempts to group voxels
 *  up, thus drastically reducing the amount of memory needed to store large worlds. This
 *  particular implementation splits the world into a giant octree to aid in storing and
 *  looking up groups.
 * \note When splitting odd sized octants, this particular implementation always favors
 *  the lower half, granting it the larger half. This is VERY important to remember when
 *  determining which octant a location falls into or when dtermining the midway point of
 *  a voxel group. */
class OctreeVoxelGrid : public VoxelGrid {
public:
    /*! Creates a new OctreeVoxelGrid.
     * \param width    The width  of the grid.
     * \param height   The height of the grid.
     * \param depth    The depth  of the grid.
     * \param position Represents the location of the lowest corner contained by this group.
     * \param index    The VoxelPalette index this group contains.
     * \param parent   This groups parent group (useful for pruning groups). */
    OctreeVoxelGrid(int width, int height, int depth, const Vector3 &position,
        PaletteIndex index, OctreeVoxelGrid* parent);

    /*! OctreeVoxelGrid's destructor */
    virtual ~OctreeVoxelGrid();

#pragma mark VoxelGrid interface function declarations.

    /*! Sets the voxel type at the given location. */
    virtual void setPaletteIndex(int x, int y, int z, PaletteIndex type);

    /*! Returns the voxel at the given location. */
    virtual PaletteIndex getPaletteIndex(int x, int y, int z);

    /*! Gets the maximum, full z level at the given x/y location. */
    virtual int getSurfaceLevel(int x, int y);

    /*! Saves the VoxelGrid to the given IOTarget. */
    virtual void save(IOTarget *target);

    /*! Loads the VoxelGrid from the given IOTarget. */
    virtual void load(IOTarget *target);

    /*! Deletes all of this group's children. */
    virtual void clear();

#pragma mark OctreeVoxelGrid specific functions.
    /*! Takes position and dimensions of an octant somewhere in the OctreeVoxelGrid
     *  and attempts to add it, returning false if it fails. */
    void addOctant(int width, int height, int depth, const Vector3 &position, PaletteIndex type);

    /*! Debugging function to see the current state of the octree. */
    void examineOctree(int depth = 0);

    /*! Prints out some simple data on the group. */
    void printStats();

protected:
    /*! Saves this specific group and all children, returning the number of groups saved. */
    int saveGroup(IOTarget *target);

    /*! Sets the voxel type at the given location. */
    void setPaletteIndex(const Vector3 &loc, PaletteIndex newIndex);

    /*! Searches downward through the octree for the lowest existing group that contains
     *  the given location. */
    OctreeVoxelGrid* getLowestGroup(const Vector3 &loc);

    /*! Returns the type of this group. */
    PaletteIndex getType();
    PaletteIndex getType(PaletteIndex tData);
    PaletteIndex defaultType();

    /*! Returns true if this OctreeVoxelGrid has no children. */
    bool isLeaf();

    /*! Returns true if this OctreeVoxelGrid represents exactly 1 voxel. */
    bool isSmallest();

    /*! Because this implementation supports an odd sized world, not all OctreeVoxelGrids will
     *  actually be using all 8 octants. This functions determines if the current
     *  OctreeVoxelGrid supports the octant at the given index. */
    bool hasOctant(int index);

    /*! This examines all children and, if possible, will optimize memory usage by
     *  pruning children with no value or switching its base group to reduce the number of
     *  children it must support.
     *
     *  Pruning children: If all children of this OctreeVoxelGrid have the same type as this
     *  voxel group, there is no reason for the children to exist as this OctreeVoxelGrid already
     *  adequately represents them.
     *
     *  Changing the base group: If this voxel represents air, but has 7 sediment children,
     *  then space can be saved by switching this OctreeVoxelGrid over to sediment, deleting the
     *  7 sediment children, and creating a single child of type air. */
    bool optimizeGroup();

    /*! Sets the type of this group. */
    void setType(PaletteIndex type);

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

    /*! These functions return the low value in this OctreeVoxelGrid. */
    inline int lowX() { return _pos.x; }
    inline int lowY() { return _pos.y; }
    inline int lowZ() { return _pos.z; }

    /*! These functions return the splitting point of this OctreeVoxelGrid. */
    inline int midX() { return lowX() + lowerWidth();  }
    inline int midY() { return lowY() + lowerHeight(); }
    inline int midZ() { return lowZ() + lowerDepth();  }

private:
    struct Chunk;
    friend struct Chunk;

    class OctreeVoxelGridPool {
    public:
        OctreeVoxelGridPool(int initialSize, OctreeVoxelGrid *parent);
        virtual ~OctreeVoxelGridPool();

        OctreeVoxelGrid* getOctreeVoxelGrid(int width, int height, int depth,
            const Vector3 &position, PaletteIndex voxel, OctreeVoxelGrid* parent);

        void putOctreeVoxelGrid(OctreeVoxelGrid *&group);

        void printStats();

        OctreeVoxelGrid *getParent();

    protected:
        void createOctreeVoxelGrid();

        OctreeVoxelGrid *_parent;
        Chunk *_basePtr;
        Chunk *_freePtr;
        Chunk *_lastPtr;

        int _available;
        int _used;

    };

    /*! Creates an empty group using the given OctreeVoxelGridPool to pull new children from. */
    OctreeVoxelGrid();

    /*! Sets the group's pool. */
    void setPool(OctreeVoxelGridPool *pool);

    /*! Initializes the object with the appropriate values. */
    OctreeVoxelGrid* initialize(int width, int height, int depth, const Vector3 &position,
        PaletteIndex type, OctreeVoxelGrid* parent);

private:
    Vector3 _pos;   //!< This group's position and dimensions.
    PaletteIndex _voxel; //!< Storage for voxel information.

    OctreeVoxelGridPool *_pool;    //!< The pool this group will pull children from.
    OctreeVoxelGrid* _children[8]; //!< The 8 possible children of this group.
    OctreeVoxelGrid* _parent;      //!< The parent of this OctreeVoxelGrid.

};

#endif
