/*
 * MHLookNFeelBindings.h
 * Mountainhome
 *
 * Created by datalurkur on 2/21/2011
 * Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHLOOKNFEELBINDINGS_H_
#define _MHLOOKNFEELBINDINGS_H_

#include "MHLookNFeel.h"
#include "RubyBindings.h"

class MHLookNFeelBindings : public RubyBindings<MHLookNFeel, true> {
public:
    MHLookNFeelBindings();
    virtual ~MHLookNFeelBindings() {}
};

#endif
