/*
 * RenderableBindings.cpp
 * Mountainhome
 *
 * Created by datalurkur on 3/30/11.
 * Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#include "RenderableBindings.h"

RenderableBindings::RenderableBindings()
: RubyBindings<Renderable, true>(
	rb_define_class("Renderable", rb_cObject),
	"RenderableBindings")
{ }
