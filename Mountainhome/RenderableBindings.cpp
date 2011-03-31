/*
 * RenderableBindings.cpp
 * Mountainhome
 *
 * Created by datalurkur on 3/30/11.
 * Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#include "RenderableBindings.h"
#include "Matrix.h"

RenderableBindings::RenderableBindings()
: RubyBindings<Renderable, true>(
	rb_define_class("Renderable", rb_cObject),
	"RenderableBindings")
{
    rb_define_method(_class, "set_translation_matrix", RUBY_METHOD_FUNC(RenderableBindings::SetTranslationMatrix), 3);
}

VALUE RenderableBindings::SetTranslationMatrix(VALUE rSelf, VALUE rX, VALUE rY, VALUE rZ) {
    Renderable *cSelf = Get()->getPointer(rSelf);
    cSelf->setModelMatrix(Matrix::Translation(Vector3(NUM2DBL(rX), NUM2DBL(rY), NUM2DBL(rZ))));
    return rSelf;
}
