#include "OptionsModule.h"

OptionsModule::OptionsModule(ResourceGroupManager *rManager): _rManager(rManager) { }

OptionsModule::~OptionsModule() { }

void OptionsModule::load(std::string filename) {
    _filename = _rManager->findResource(filename);
    if(_filename.length() > 0) {
        Info("Reading property tree from " << _filename);
        read_ini(filename, _ptree);
    }
    else {
        Error("Could not load options file " << filename);
    }
}

void OptionsModule::save() {
}

