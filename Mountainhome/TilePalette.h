/*
 * TilePalette.h
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

class Tile {
public:
    Tile();
    Tile(const Tile &otherTile);
    ~Tile();

    VALUE getType() const;
    void setType(VALUE type);

    void duplicate(const Tile &otherTile);
    void copyParameters(ParameterMap &map) const;
    int numParameters() const;

    bool hasParameter(ParameterID id) const;
    const ParameterData &getParameter(ParameterID id) const;
    void addParameter(ParameterID id, const ParameterData &value);
    void setParameter(ParameterID id, const ParameterData &value);
    bool isParameterEqual(const ParameterData &thisParameter, const ParameterData &otherParameter) const;

    void operator=(const Tile &other);
    bool operator==(const Tile &other) const;

private:
    ParameterMap _parameters;
    VALUE _rubyType;

};

class TilePalette {
public:
    static const int EmptyTile = -1;

public:
    TilePalette();
    ~TilePalette();

    /*! Gets the PaletteIndex associated with the given Tile. If the given Tile is not
     * egistered, EmptyTile will be returned. */
    PaletteIndex getPaletteIndex(const Tile &tile);

    /*! Gets the Material associated with the given PaletteIndex. If the given index does
     *  not exist, NULL is returned instead. */
    Material * getMaterialForIndex(PaletteIndex index);

    /*! Gets the Tile associated with the given PaletteIndex. If the given index does not
     *  exist, NULL is returned instead. */
    const Tile * getTileForIndex(PaletteIndex index);

    /*! Associates a tile and material with a new PaletteIndex.
     * \note This does no safety checking to avoid duplicate Tile/Mat entries. */
    PaletteIndex registerTile(const std::string &name, const Tile &tile, Material *mat);

private:
    std::vector <Tile> _registeredTypes;
    std::vector <Material*> _registeredMaterials;
};

#endif
