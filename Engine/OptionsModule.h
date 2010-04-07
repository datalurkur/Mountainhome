#ifndef _OPTIONSMODULE_H_
#define _OPTIONSMODULE_H_

#include <string>

#include <Base/PTreeResourceFactory.h>
#include <Base/ResourceGroupManager.h>

class OptionsModule {
public:
    OptionsModule(ResourceGroupManager *rManager);
    virtual ~OptionsModule();

    void load(std::string filename);
    void save();

protected:
    std::string _filename;
    boost::property_tree::ptree _ptree;
    ResourceGroupManager *_rManager;
};

#endif

