/*
 *  TileBindings.cpp
 *  Mountainhome
 *
 *  Created by apple on 11/13/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "TileBindings.h"

VALUE TileBindings::_rubyLookup = Qnil;

TileBindings::TileBindings() {
    VALUE _module = rb_define_module("TileParameter");
    
    // Import parameters from Tile.h.
    
    rb_define_singleton_method(_module, "method_missing",  RUBY_METHOD_FUNC(TileBindings::Method_Missing),  -1);
    
    rb_define_class_variable(_module, "@@lookup", rb_hash_new());
    _rubyLookup = rb_cv_get(_module, "@@lookup");
    
    rb_hash_aset(_rubyLookup, ID2SYM(rb_intern("Liquid")), INT2NUM(LIQUID));
    rb_hash_aset(_rubyLookup, ID2SYM(rb_intern("Unknown1")), INT2NUM(UNKNOWN1));
    rb_hash_aset(_rubyLookup, ID2SYM(rb_intern("Unknown2")), INT2NUM(UNKNOWN2));
    rb_hash_aset(_rubyLookup, ID2SYM(rb_intern("Unknown3")), INT2NUM(UNKNOWN3));
}

VALUE TileBindings::Method_Missing(int argc, VALUE * argv, VALUE self) {
    VALUE k = argv[0];
    VALUE v = rb_hash_aref(_rubyLookup, k);
    /* If rb_hash_aref returned Qnil, the key k doesn't have a hash entry,
     so defer up the missing_method chain. */
    if(v != Qnil) {
        return v;
    } else {
        return rb_call_super(argc, argv);
    }
}