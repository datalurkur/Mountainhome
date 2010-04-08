#include <Base/FileSystem.h>
#include "OptionsModule.h"

OptionsModule::OptionsModule(const std::string &homeDirectory, const std::string &name) {
    _filename = homeDirectory + name;
    FileSystem::FormatPath(_filename);
}

OptionsModule::~OptionsModule() { }

void OptionsModule::load() {
    if (FileSystem::Exists(_filename)) {
        read_ini(_filename, _ptree);
    } else {
        // Save the default settings to disk.
        save();
    }
}

void OptionsModule::save() {

}

void OptionsModule::apply() {

}

