/*
 * TilePalette.cpp
 * Mountainhome
 *
 * Created by Andrew Jean on 12/4/10
 * Copyright 2010 Mountainhome Project. All rights reserved.
 */

#include "TilePalette.h"
#include "Assertion.h"
#include <Content/Content.h>

std::ostream& operator<<(std::ostream &lhs, const Tile &rhs) {
    lhs << "Tile" << " ( ";
    lhs << "\nMaterial:     " << (int)boost::any_cast<char>(rhs.getProperty(MATERIAL));
    lhs << "\nLiquid:       " << boost::any_cast<bool>(rhs.getProperty(LIQUID));
    lhs << "\nSelected:     " << boost::any_cast<bool>(rhs.getProperty(SELECTED));
    lhs << "\nLiquid Level: " << (int)boost::any_cast<char>(rhs.getProperty(LIQUID_LEVEL));
    lhs << "\n )";
    return lhs;
}

TilePalette::TilePalette() { }
TilePalette::~TilePalette() { }

const PropertyType &TilePalette::getProperty(PaletteIndex index, TileProperty property) const {
// #ifdef _DEBUG
    if(index >= 0 && index < _registeredTypes.size()) {
// #endif
        return _registeredTypes[index].getProperty(property);
// #ifdef _DEBUG
    }
    else {
        // There's really no reason to ask an empty tile for its properties
        Warn("Care should be taken to ensure that empty tiles aren't queried for properties.");
        return _defaultTile.getProperty(property);
    }
// #endif
}

PaletteIndex TilePalette::setProperty(PaletteIndex index, TileProperty property, PropertyType value) {
    Tile newTile;

    // Make copy of the Tile in question
    if(index >= 0 && index < _registeredTypes.size()) {
        newTile = _registeredTypes[index];
    }

    // Return early if no change is made
    if(newTile.isPropertyEqual(property, &value)) { return index; }
    
    // Otherwise, change the property and find the new Tile's index
    else {
        newTile.setProperty(property, value);
        for(int i=0; i<_registeredTypes.size(); i++) {
            if(_registeredTypes[i] == newTile) { return i; }
        }
        Info("Creating a new tile palette at index " << _registeredTypes.size() << ": " << newTile);
        _registeredTypes.push_back(newTile);
        return _registeredTypes.size()-1;
    }
}

MaterialIndex TilePalette::registerTileMaterial(const std::string &materialName) {
    Material *mat = Content::GetOrLoad<Material>(materialName);
    if (mat == NULL) {
        Error("Material " << materialName << " not found!");
        return -1;
    }
    for (int i = 0; i < _registeredMaterials.size(); i++) {
        if((_registeredMaterials[i]) == mat) { return i; }
    }
    _registeredMaterials.push_back(mat);
    Info("Registered material " << materialName << " to index " << _registeredMaterials.size()-1);
    return _registeredMaterials.size()-1;
}

Material* TilePalette::getMaterialForPalette(PaletteIndex index) {
    const PropertyType &mProp = getProperty(index, MATERIAL);
    MaterialIndex mIndex = (MaterialIndex)boost::any_cast<char>(mProp);

    if(_registeredMaterials.size() == 0) {
        ASSERT(!"No materials yet specified for palette.");
    }
    return _registeredMaterials[mIndex];
}