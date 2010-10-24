#include "RubyBindings.h"

#ifndef _MHSELECTION_H_
#define _MHSELECTION_H_

class MHSelection : public RubyBindings<MHSelection, true> {
public:
    MHSelection();
    virtual ~MHSelection();

    static void SetupBindings();
    //void initialize(std::list <MHTypeObject*> &selected) {}

private:
    //std::list <MHTypeObject*> _selected;
};

#endif
