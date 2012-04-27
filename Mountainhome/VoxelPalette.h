/*
 * VoxelPalette.h
 * Mountainhome
 *
 * Created by Andrew Jean on 12/4/10
 * Copyright 2010 Mountainhome Project. All rights reserved.
 */

#ifndef _TILEPALETTE_H_
#define _TILEPALETTE_H_

#include <Boost/any.hpp>
#include <vector>
#include <string>
#include "Assertion.h"

// Include this for use of VALUE, for some reason including ruby.h results in a redefinition of struct timespec
#include "RubyBindings.h"

class Material;

typedef char PaletteIndex;

typedef std::string ParameterID;
typedef boost::any ParameterData;
typedef std::map<ParameterID, ParameterData> ParameterMap;
typedef ParameterMap::iterator ParameterIterator;
typedef ParameterMap::const_iterator ConstParameterIterator;

class Voxel {
public:
    Voxel();
    Voxel(const Voxel &otherVoxel);
    ~Voxel();

    VALUE getType() const;
    void setType(VALUE type);

    void duplicate(const Voxel &otherVoxel);
    void copyParameters(ParameterMap &map) const;
    int numParameters() const;

    bool hasParameter(ParameterID id) const;
    const ParameterData &getParameter(ParameterID id) const;
    void addParameter(ParameterID id, const ParameterData &value);
    void setParameter(ParameterID id, const ParameterData &value);
    bool isParameterEqual(const ParameterData &thisParameter, const ParameterData &otherParameter) const;

    void operator=(const Voxel &other);
    bool operator==(const Voxel &other) const;

private:
    ParameterMap _parameters;
    VALUE _rubyType;

};

class VoxelPalette {
public:
    static const int EmptyVoxel = -1;

public:
    VoxelPalette();
    ~VoxelPalette();

    /*! Gets the PaletteIndex associated with the given Voxel. If the given Voxel is not
     * egistered, EmptyVoxel will be returned. */
    PaletteIndex getPaletteIndex(const Voxel &voxel);

    /*! Gets the Material associated with the given PaletteIndex. If the given index does
     *  not exist, NULL is returned instead. */
    Material * getMaterialForIndex(PaletteIndex index);

    /*! Gets the Voxel associated with the given PaletteIndex. If the given index does not
     *  exist, NULL is returned instead. */
    const Voxel * getVoxelForIndex(PaletteIndex index);

    /*! Associates a voxel and material with a new PaletteIndex.
     * \note This does no safety checking to avoid duplicate Voxel/Mat entries. */
    PaletteIndex registerVoxel(const std::string &name, const Voxel &voxel, Material *mat);

private:
    std::vector <Voxel> _registeredTypes;
    std::vector <Material*> _registeredMaterials;
};

#endif
