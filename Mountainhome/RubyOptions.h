/*
 *  RubyOptions.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _RUBYOPTIONS_H_
#define _RUBYOPTIONS_H_

#include "OptionsModule.h"
#include "RubyBindings.h"

class RubyOptions: public RubyBindings<OptionsModule, false> {
public:
    static void SetupBindings();
};

#endif
