#include "TilePalette.h"
#include "Assertion.h"
#include <Render/MaterialManager.h>

TilePalette::TilePalette(): _manager(0) { }
TilePalette::TilePalette(MaterialManager *manager): _manager(manager) { }
TilePalette::~TilePalette() { }

PropertyType* TilePalette::getProperty(PaletteIndex index, TileProperty property) {
    // TODO - Make this return a non static type
    if(index >= 0 && index < _registeredTypes.size()) {
        return _registeredTypes[index].getProperty(property);
    } else {
        return NULL;
    }
}

PaletteIndex TilePalette::setProperty(PaletteIndex index, TileProperty property, PropertyType value) {
    Tile newTile;

    // Make copy of the Tile in question
    if(index >= 0 && index < _registeredTypes.size()) {
        newTile = _registeredTypes[index];
    }

    // Return early if no change is made
    if(newTile.hasProperty(property, &value)) { return index; }
    
    // Otherwise, change the property and find the new Tile's index
    else {
        newTile.setProperty(property, value);
        for(int i=0; i<_registeredTypes.size(); i++) {
            if(_registeredTypes[i] == newTile) { return i; }
        }
        Info("Creating a new tile palette at index " << _registeredTypes.size());
        _registeredTypes.push_back(newTile);
        return _registeredTypes.size()-1;
    }
}

MaterialIndex TilePalette::registerTileMaterial(const std::string &materialName) {
    Material *mat = _manager->getCachedResource(materialName);
    if(mat==NULL) {
        Error("Material " << materialName << " not found!");
        return 0;
    }
    for(int i=0; i < _registeredMaterials.size(); i++) {
        if((_registeredMaterials[i]) == mat) { return i; }
    }
    _registeredMaterials.push_back(mat);
    Info("Registered material " << materialName << " to index " << _registeredMaterials.size()-1);
    return _registeredMaterials.size()-1;
}

Material* TilePalette::getMaterialForPalette(PaletteIndex index) {
    PropertyType *mProp = getProperty(index, MATERIAL);
    MaterialIndex mIndex;
    if(mProp) {
        mIndex = (MaterialIndex)boost::any_cast<char>(*mProp);
    }
    else if(_registeredMaterials.size() > 0) {
        mIndex = 0;
        Info("Material not specified for palette index " << index);
    }
    else {
        Info("No materials yet specified for palette.");
        ASSERT(1);
    }
    return _registeredMaterials[mIndex];
}
