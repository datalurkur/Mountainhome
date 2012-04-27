/*
 * VoxelPalette.cpp
 * Mountainhome
 *
 * Created by Andrew Jean on 12/4/10
 * Copyright 2010 Mountainhome Project. All rights reserved.
 */

#include "VoxelPalette.h"
#include "Assertion.h"

#include <Content/Content.h>
#include <Content/MaterialManager.h>

////////////////////////////////////
#pragma mark VoxelPalette definitions
////////////////////////////////////

VoxelPalette::VoxelPalette() { }
VoxelPalette::~VoxelPalette() {
    // Destroy the materials created
    std::vector<Material*>::iterator itr = _registeredMaterials.begin();
    for(int i = 0; itr != _registeredMaterials.end(); itr++, i++) {
        // FIXME: MAGICAL NAME TAKEN FROM TerrainBINDINGS.CPP
        std::stringstream name;
        name << "voxel palette entry [" << (int)i << "]";
        Content::GetMaterialManager()->unloadResource(name.str());
    }
}

PaletteIndex VoxelPalette::getPaletteIndex(const Voxel &voxel) {
    for(int i = 0; i<_registeredTypes.size(); i++) {
        if(voxel == _registeredTypes[i]) { return i; }
    }

    return EmptyVoxel;
}

const Voxel * VoxelPalette::getVoxelForIndex(PaletteIndex index) {
    return _registeredTypes.size() > (int)index ? &(_registeredTypes[index]) : NULL;
}

Material *VoxelPalette::getMaterialForIndex(PaletteIndex index) {
    return _registeredTypes.size() > (int)index ? _registeredMaterials[index] : NULL;
}

PaletteIndex VoxelPalette::registerVoxel(const std::string &name, const Voxel &voxel, Material *mat) {
    // Push the new voxel onto the palette and return its index
    Info("Registering voxel " << name);
    _registeredTypes.push_back(voxel);
    _registeredMaterials.push_back(mat);
    return _registeredTypes.size() - 1;
}

/////////////////////////////
#pragma mark Voxel definitions
/////////////////////////////
Voxel::Voxel() { }
Voxel::Voxel(const Voxel &otherVoxel) {
    duplicate(otherVoxel);
}
Voxel::~Voxel() {
    _parameters.clear();
}

VALUE Voxel::getType() const { return _rubyType; }
void Voxel::setType(VALUE type) { _rubyType = type; }

void Voxel::duplicate(const Voxel &otherVoxel) {
    otherVoxel.copyParameters(_parameters);
    _rubyType = otherVoxel.getType();
}

void Voxel::copyParameters(ParameterMap &map) const {
    ConstParameterIterator itr = _parameters.begin();
    for(; itr != _parameters.end(); itr++) {
        map[(*itr).first] = (*itr).second;
    }
}

int Voxel::numParameters() const {
    return _parameters.size();
}

bool Voxel::hasParameter(ParameterID id) const {
    return (_parameters.find(id) != _parameters.end());
}

const ParameterData & Voxel::getParameter(ParameterID id) const {
#if DEBUG
    ASSERT(hasParameter(id));
#endif
    return _parameters.find(id)->second;
}

void Voxel::addParameter(ParameterID id, const ParameterData &value) {
    _parameters[id] = value;
}

void Voxel::setParameter(ParameterID id, const ParameterData &value) {
#if DEBUG
    ASSERT(hasParameter(id));
#endif
    _parameters[id] = value;
}

bool Voxel::isParameterEqual(const ParameterData &thisParameter, const ParameterData &otherParameter) const {
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

void Voxel::operator=(const Voxel &other) {
    duplicate(other);
}

bool Voxel::operator==(const Voxel &other) const {
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

