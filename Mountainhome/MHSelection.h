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
#include "Vector.h"

class MHSelection {
public:
    MHSelection();
    virtual ~MHSelection();

    void clear();

    void append(MHActor *actor);
    void append(Vector3 &tile);

    void remove(Vector3 tile);

    int numSelectedActors();
    int numSelectedTiles();

    std::list <MHActor*> &getSelectedActors();
    std::list <Vector3> &getSelectedTiles();

private:
    std::list <MHActor*> _selectedActors;
    std::list <Vector3> _selectedTiles;
};
#endif
