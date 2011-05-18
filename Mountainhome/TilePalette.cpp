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

////////////////////////////////////
#pragma mark TilePalette definitions
////////////////////////////////////

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

    return EmptyTile;
}

const Tile * TilePalette::getTileForIndex(PaletteIndex index) {
    return _registeredTypes.size() > (int)index ? &(_registeredTypes[index]) : NULL;
}

Material *TilePalette::getMaterialForIndex(PaletteIndex index) {
    return _registeredTypes.size() > (int)index ? _registeredMaterials[index] : NULL;
}

PaletteIndex TilePalette::registerTile(const std::string &name, const Tile &tile, Material *mat) {
    // Push the new tile onto the palette and return its index
    Info("Registering tile " << name);
    _registeredTypes.push_back(tile);
    _registeredMaterials.push_back(mat);
    return _registeredTypes.size() - 1;
}

/////////////////////////////
#pragma mark Tile definitions
/////////////////////////////
Tile::Tile() { }
Tile::Tile(const Tile &otherTile) {
    duplicate(otherTile);
}
Tile::~Tile() {
    _parameters.clear();
}

VALUE Tile::getType() const { return _rubyType; }
void Tile::setType(VALUE type) { _rubyType = type; }

void Tile::duplicate(const Tile &otherTile) {
    otherTile.copyParameters(_parameters);
    _rubyType = otherTile.getType();
}

void Tile::copyParameters(ParameterMap &map) const {
    ConstParameterIterator itr = _parameters.begin();
    for(; itr != _parameters.end(); itr++) {
        map[(*itr).first] = (*itr).second;
    }
}

int Tile::numParameters() const {
    return _parameters.size();
}

bool Tile::hasParameter(ParameterID id) const {
    return (_parameters.find(id) != _parameters.end());
}

const ParameterData & Tile::getParameter(ParameterID id) const {
#if DEBUG
    ASSERT(hasParameter(id));
#endif
    return _parameters.find(id)->second;
}

void Tile::addParameter(ParameterID id, const ParameterData &value) {
    _parameters[id] = value;
}

void Tile::setParameter(ParameterID id, const ParameterData &value) {
#if DEBUG
    ASSERT(hasParameter(id));
#endif
    _parameters[id] = value;
}

bool Tile::isParameterEqual(const ParameterData &thisParameter, const ParameterData &otherParameter) const {
    if(thisParameter.type() != otherParameter.type()) { return false; }
    else if(thisParameter.type() == typeid(bool)) {
        return (boost::any_cast<bool>(thisParameter) ==
                boost::any_cast<bool>(otherParameter));
    }
    else if(thisParameter.type() == typeid(char)) {
        return (boost::any_cast<char>(thisParameter) ==
                boost::any_cast<char>(otherParameter));
    }
    else if(thisParameter.type() == typeid(std::string)) {
        return (boost::any_cast<std::string>(thisParameter) ==
                boost::any_cast<std::string>(otherParameter));
    }
    else if(thisParameter.type() == typeid(int)) {
        return (boost::any_cast<int>(thisParameter) ==
                boost::any_cast<int>(otherParameter));
    }
    else if(thisParameter.type() == typeid(double)) {
        return (boost::any_cast<double>(thisParameter) ==
                boost::any_cast<double>(otherParameter));
    }
    else {
        Error("Can't compare properties");
    }
    return false;
}

void Tile::operator=(const Tile &other) {
    duplicate(other);
}

bool Tile::operator==(const Tile &other) const {
    if(_rubyType != other.getType()) { return false; }
    if(_parameters.size() != other.numParameters()) { return false; }

    ConstParameterIterator itr = _parameters.begin();
    for(; itr != _parameters.end(); itr++) {
        if(!other.hasParameter((*itr).first)) { return false; }

        const ParameterData &otherData = other.getParameter((*itr).first);
        if(!isParameterEqual((*itr).second, otherData)) { return false; }
    }
    return true;
}

