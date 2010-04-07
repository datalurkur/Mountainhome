#ifndef _RUBYOPTIONS_H_
#define _RUBYOPTIONS_H_

#include "OptionsModule.h"
#include "RubyBindings.h"

class RubyOptions: public RubyBindings<OptionsModule, false> {
public:
    static void SetupBindings();
};

#endif
