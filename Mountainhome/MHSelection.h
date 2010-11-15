/*
 *  MHSelection.h
 *  Mountainhome
 *
 *  Created by datalurkur on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHSELECTION_H_
#define _MHSELECTION_H_

#include "MHActor.h"

class MHSelection {
public:
    MHSelection();
    virtual ~MHSelection();

    void initialize(std::list <SceneNode*> selected);
    std::list <MHActor*> &getSelected();

private:
    std::list <MHActor*> _selected;
};

#endif
