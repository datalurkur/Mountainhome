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
#include <Content/MaterialManager.h>

//std::ostream& operator<<(std::ostream &lhs, const Tile &rhs) {
//    lhs << "Tile " << rhs.getShaderName() << "/" << rhs.getTextureName();
//    return lhs;
//}

TilePalette::TilePalette() { }
TilePalette::~TilePalette() {
    // Destroy the materials created
    std::vector<Material*>::iterator itr = _registeredMaterials.begin();
    for(int i = 0; itr != _registeredMaterials.end(); itr++, i++) {
        // FIXME: MAGICAL NAME TAKEN FROM MHTERRAINBINDINGS.CPP
        std::string matName = std::string("tile palette entry [" + i) + "]";
        Content::GetMaterialManager()->unloadResource(matName);
    }
}

PaletteIndex TilePalette::getPaletteIndex(const Tile &tile) {
    for(int i = 0; i<_registeredTypes.size(); i++) {
        if(tile == _registeredTypes[i]) { return i; }
    }

    return IndexNotFound;
}

const Tile &TilePalette::getTileForIndex(PaletteIndex index) {
#if DEBUG
    ASSERT(_registeredTypes.size() > (int)index);
#endif
    return _registeredTypes[(int)index];
}

Material *TilePalette::getMaterialForIndex(PaletteIndex index) {
#if DEBUG
    ASSERT(_registeredTypes.size() > (int)index);
#endif
    return _registeredMaterials[(int)index];
}

int TilePalette::registerTile(const std::string &name, Tile &tile, Material *mat) {
    // Push the new tile onto the palette and return its index
    Info("Registering tile " << name);
    _registeredTypes.push_back(tile);
    _registeredMaterials.push_back(mat);
    return _registeredTypes.size()-1;
}
