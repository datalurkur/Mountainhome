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

#include "Material.h"
#include "MaterialManager.h"

std::ostream& operator<<(std::ostream &lhs, const Tile &rhs) {
    lhs << "Tile " << rhs.getShaderName() << "/" << rhs.getTextureName();
    return lhs;
}

TilePalette::TilePalette() { }
TilePalette::~TilePalette() {
    // Destroy the materials created
    std::vector<Material*>::iterator itr = _registeredMaterials.begin();
    for(int c = 0; itr != _registeredMaterials.end(); itr++, c++) {
        // Unload the resource and delete its data
        std::string matName = "palette";
        matName += c;
        Content::GetMaterialManager()->unloadResource(matName);
    }
}

PaletteIndex TilePalette::getPaletteIndex(Tile &tile) {
    for(int i=0; i<_registeredTypes.size(); i++) {
        if(tile == _registeredTypes[i]) { return i; }
    }
    return registerTile(tile);
}

const Tile &TilePalette::getTileForIndex(PaletteIndex index) {
#if DEBUG
    ASSERT(_registeredTypes.size() > (int)index);
#endif
    return _registeredTypes[(int)index];
}

int TilePalette::registerTile(Tile &tile) {
    // Begin by creating a new material for this tile
    Material *newMat = new Material();
    Shader *shader = Content::GetOrLoad<Shader>(tile.getShaderName());
    newMat->setShader(shader);
    newMat->setShaderParameter("colorMap", Content::GetOrLoad<Texture>(tile.getTextureName()));
    tile.setMaterial(newMat);

    // Deal with changing the color of selected tiles
    if(tile.hasParameter("selected") && boost::any_cast<bool>(tile.getParameter("selected")) == true) {
        newMat->setShaderParameter("color", new Vector4(1.0, 0.0, 0.0, 0.0));
    } else {
        newMat->setShaderParameter("color", new Vector4(1.0, 1.0, 1.0, 1.0));
    }

    // Keep track of the material so we can delete it later
    std::string matName = "palette";
    matName += _registeredTypes.size();
    Content::GetMaterialManager()->registerResource(matName, newMat);
    _registeredMaterials.push_back(newMat);

    // Push the new tile onto the palette and return its index
    Info("Registering tile " << tile);
    _registeredTypes.push_back(tile);
    return _registeredTypes.size()-1;
}
