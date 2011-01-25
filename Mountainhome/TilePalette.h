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

typedef char MaterialIndex;
typedef char PaletteIndex;
typedef char TileProperty;

class Material;
class MaterialManager;

enum {
    MATERIAL = 0,
    LIQUID,
    SELECTED,
    LIQUID_LEVEL,
    PROPERTY_COUNT
};

enum {
    TILE_EMPTY = -1
};

typedef boost::any PropertyType;

struct Tile {
    PropertyType **_propArray;

    Tile() {
        init();
        _propArray[MATERIAL]     = new PropertyType((char)-1);
        _propArray[LIQUID]       = new PropertyType((bool)false);
        _propArray[SELECTED]     = new PropertyType((bool)false);
        _propArray[LIQUID_LEVEL] = new PropertyType((char)10);
    }
    Tile(const Tile &otherTile) {
        init();
        duplicate(otherTile);
    }
    ~Tile() {
        for(int i=0; i<PROPERTY_COUNT; i++) {
            if(_propArray[i] != NULL) {
                delete _propArray[i];
                _propArray[i] = NULL;
            }
        }
        free(_propArray);
    }

    void init() { _propArray = (PropertyType**)calloc(sizeof(PropertyType*), PROPERTY_COUNT); }
    void duplicate(const Tile &otherTile) {
        for(int i=0; i<PROPERTY_COUNT; i++) {
            const PropertyType &otherProp = otherTile.getProperty(i);
            setProperty(i, otherProp);
        }
    }

    const PropertyType &getProperty(TileProperty property) const { return *_propArray[property]; }
    void setProperty(TileProperty property, const PropertyType &value) {
        if(_propArray[property] != NULL) { delete _propArray[property]; }
        _propArray[property] = new PropertyType(value);
    }

    bool isPropertyEqual(TileProperty property, const PropertyType &otherProperty) const {
        const PropertyType &thisProperty = getProperty(property);
        
        if(thisProperty.type() != otherProperty.type()) { return false; }
        else if(thisProperty.type() == typeid(bool)) {
            return (boost::any_cast<bool>(thisProperty) ==
                    boost::any_cast<bool>(otherProperty));
        }
        else if(thisProperty.type() == typeid(char)) {
            return (boost::any_cast<char>(thisProperty) ==
                    boost::any_cast<char>(otherProperty));
        }
        else if(thisProperty.type() == typeid(std::string)) {
            return (boost::any_cast<std::string>(thisProperty) ==
                    boost::any_cast<std::string>(otherProperty));
        }
        else {
            __asm int 3;
        }
        return false;
    }

    void operator=(const Tile &other) {
        duplicate(other);
    }

    bool operator==(const Tile &other) const {
        for(int i=0; i<PROPERTY_COUNT; i++) {
            if(!isPropertyEqual(i, other.getProperty(i))) { return false; }
        }
        return true;
    }
};

class TilePalette {
public:
    TilePalette();
    TilePalette(MaterialManager *manager);
    ~TilePalette();

    const PropertyType &getProperty(PaletteIndex index, TileProperty property) const;
    PaletteIndex setProperty(PaletteIndex index, TileProperty property, PropertyType value);

    MaterialIndex registerTileMaterial(const std::string &materialName);
    Material *getMaterialForPalette(PaletteIndex index) ;

private:
    std::vector <Material*> _registeredMaterials;
    std::vector <Tile> _registeredTypes;

    MaterialManager *_manager;

    Tile _defaultTile;
};

#endif
