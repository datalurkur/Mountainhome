/*
 * MHLookNFeelBindings.cpp
 * Mountainhome
 *
 * Created by datalurkur on 2/21/2011
 * Copyright 2011 Mountainhome Project. All rights reserved.
 * 
 */

#include "MHLookNFeelBindings.h"

MHLookNFeelBindings::MHLookNFeelBindings()
: RubyBindings<MHLookNFeel, true>(
    rb_define_class("MHLookNFeel", rb_cObject),
    "MHLookNFeelBindings")
{
    rb_define_alloc_func(_class, MHLookNFeelBindings::Alloc<MHLookNFeelBindings>);
}
