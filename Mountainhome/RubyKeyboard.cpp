/*
 *  RubyKeyboard.cpp
 *  Mountainhome
 *
 *  Created by Paul on 3/8/10.
 *  Copyright 2010 MHCore Project. All rights reserved.
 *
 */

#include <Engine/Keyboard.h>
#include "RubyKeyboard.h"

VALUE RubyKeyboard::Class = NULL;
VALUE RubyKeyboard::RubyLookup = NULL;

VALUE RubyKeyboard::Method_Missing(int argc, VALUE * argv, VALUE self) {
    VALUE k = argv[0];
    VALUE v = rb_hash_aref(RubyLookup, k);
    /* If rb_hash_aref returned Qnil, the key k doesn't have a hash entry,
       so defer up the missing_method chain. */
    if(v != Qnil) {
        return v;
    } else {
        return rb_call_super(argc, argv);
    }
}

void RubyKeyboard::SetupBindings() {
    Class = rb_define_class("Keyboard", rb_cObject);
    rb_define_singleton_method(Class, "method_missing",  RUBY_METHOD_FUNC(RubyKeyboard::Method_Missing),  -1);

    rb_define_class_variable(Class, "@@lookup", rb_hash_new());
    RubyLookup = rb_cv_get(Class, "@@lookup");

    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_UNKNOWN")), INT2NUM(Keyboard::KEY_UNKNOWN));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_FIRST")), INT2NUM(Keyboard::KEY_FIRST));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_BACKSPACE")), INT2NUM(Keyboard::KEY_BACKSPACE));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_TAB")), INT2NUM(Keyboard::KEY_TAB));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_CLEAR")), INT2NUM(Keyboard::KEY_CLEAR));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_RETURN")), INT2NUM(Keyboard::KEY_RETURN));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_PAUSE")), INT2NUM(Keyboard::KEY_PAUSE));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_ESCAPE")), INT2NUM(Keyboard::KEY_ESCAPE));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_SPACE")), INT2NUM(Keyboard::KEY_SPACE));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_EXCLAIM")), INT2NUM(Keyboard::KEY_EXCLAIM));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_QUOTEDBL")), INT2NUM(Keyboard::KEY_QUOTEDBL));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_HASH")), INT2NUM(Keyboard::KEY_HASH));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_DOLLAR")), INT2NUM(Keyboard::KEY_DOLLAR));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_AMPERSAND")), INT2NUM(Keyboard::KEY_AMPERSAND));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_QUOTE")), INT2NUM(Keyboard::KEY_QUOTE));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_LEFTPAREN")), INT2NUM(Keyboard::KEY_LEFTPAREN));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_RIGHTPAREN")), INT2NUM(Keyboard::KEY_RIGHTPAREN));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_ASTERISK")), INT2NUM(Keyboard::KEY_ASTERISK));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_PLUS")), INT2NUM(Keyboard::KEY_PLUS));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_COMMA")), INT2NUM(Keyboard::KEY_COMMA));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_MINUS")), INT2NUM(Keyboard::KEY_MINUS));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_PERIOD")), INT2NUM(Keyboard::KEY_PERIOD));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_SLASH")), INT2NUM(Keyboard::KEY_SLASH));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_0")), INT2NUM(Keyboard::KEY_0));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_1")), INT2NUM(Keyboard::KEY_1));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_2")), INT2NUM(Keyboard::KEY_2));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_3")), INT2NUM(Keyboard::KEY_3));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_4")), INT2NUM(Keyboard::KEY_4));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_5")), INT2NUM(Keyboard::KEY_5));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_6")), INT2NUM(Keyboard::KEY_6));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_7")), INT2NUM(Keyboard::KEY_7));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_8")), INT2NUM(Keyboard::KEY_8));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_9")), INT2NUM(Keyboard::KEY_9));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_COLON")), INT2NUM(Keyboard::KEY_COLON));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_SEMICOLON")), INT2NUM(Keyboard::KEY_SEMICOLON));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_LESS")), INT2NUM(Keyboard::KEY_LESS));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_EQUALS")), INT2NUM(Keyboard::KEY_EQUALS));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_GREATER")), INT2NUM(Keyboard::KEY_GREATER));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_QUESTION")), INT2NUM(Keyboard::KEY_QUESTION));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_AT")), INT2NUM(Keyboard::KEY_AT));
    /* 
    Skip uppercase letters
    */
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_LEFTBRACKET")), INT2NUM(Keyboard::KEY_LEFTBRACKET));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_BACKSLASH")), INT2NUM(Keyboard::KEY_BACKSLASH));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_RIGHTBRACKET")), INT2NUM(Keyboard::KEY_RIGHTBRACKET));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_CARET")), INT2NUM(Keyboard::KEY_CARET));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_UNDERSCORE")), INT2NUM(Keyboard::KEY_UNDERSCORE));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_BACKQUOTE")), INT2NUM(Keyboard::KEY_BACKQUOTE));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_a")), INT2NUM(Keyboard::KEY_a));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_b")), INT2NUM(Keyboard::KEY_b));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_c")), INT2NUM(Keyboard::KEY_c));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_d")), INT2NUM(Keyboard::KEY_d));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_e")), INT2NUM(Keyboard::KEY_e));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_f")), INT2NUM(Keyboard::KEY_f));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_g")), INT2NUM(Keyboard::KEY_g));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_h")), INT2NUM(Keyboard::KEY_h));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_i")), INT2NUM(Keyboard::KEY_i));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_j")), INT2NUM(Keyboard::KEY_j));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_k")), INT2NUM(Keyboard::KEY_k));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_l")), INT2NUM(Keyboard::KEY_l));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_m")), INT2NUM(Keyboard::KEY_m));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_n")), INT2NUM(Keyboard::KEY_n));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_o")), INT2NUM(Keyboard::KEY_o));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_p")), INT2NUM(Keyboard::KEY_p));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_q")), INT2NUM(Keyboard::KEY_q));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_r")), INT2NUM(Keyboard::KEY_r));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_s")), INT2NUM(Keyboard::KEY_s));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_t")), INT2NUM(Keyboard::KEY_t));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_u")), INT2NUM(Keyboard::KEY_u));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_v")), INT2NUM(Keyboard::KEY_v));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_w")), INT2NUM(Keyboard::KEY_w));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_x")), INT2NUM(Keyboard::KEY_x));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_y")), INT2NUM(Keyboard::KEY_y));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_z")), INT2NUM(Keyboard::KEY_z));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_DELETE")), INT2NUM(Keyboard::KEY_DELETE));
    /* End of ASCII mapped keysyms */

    /* Numeric keypad */
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP0")), INT2NUM(Keyboard::KEY_KP0));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP1")), INT2NUM(Keyboard::KEY_KP1));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP2")), INT2NUM(Keyboard::KEY_KP2));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP3")), INT2NUM(Keyboard::KEY_KP3));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP4")), INT2NUM(Keyboard::KEY_KP4));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP5")), INT2NUM(Keyboard::KEY_KP5));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP6")), INT2NUM(Keyboard::KEY_KP6));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP7")), INT2NUM(Keyboard::KEY_KP7));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP8")), INT2NUM(Keyboard::KEY_KP8));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP9")), INT2NUM(Keyboard::KEY_KP9));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP_PERIOD")), INT2NUM(Keyboard::KEY_KP_PERIOD));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP_DIVIDE")), INT2NUM(Keyboard::KEY_KP_DIVIDE));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP_MULTIPLY")), INT2NUM(Keyboard::KEY_KP_MULTIPLY));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP_MINUS")), INT2NUM(Keyboard::KEY_KP_MINUS));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP_PLUS")), INT2NUM(Keyboard::KEY_KP_PLUS));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP_ENTER")), INT2NUM(Keyboard::KEY_KP_ENTER));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP_EQUALS")), INT2NUM(Keyboard::KEY_KP_EQUALS));

    /* Arrows + Home/End pad */
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_UP")), INT2NUM(Keyboard::KEY_UP));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_DOWN")), INT2NUM(Keyboard::KEY_DOWN));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_RIGHT")), INT2NUM(Keyboard::KEY_RIGHT));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_LEFT")), INT2NUM(Keyboard::KEY_LEFT));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_INSERT")), INT2NUM(Keyboard::KEY_INSERT));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_HOME")), INT2NUM(Keyboard::KEY_HOME));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_END")), INT2NUM(Keyboard::KEY_END));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_PAGEUP")), INT2NUM(Keyboard::KEY_PAGEUP));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_PAGEDOWN")), INT2NUM(Keyboard::KEY_PAGEDOWN));

    /* Function keys */
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F1")), INT2NUM(Keyboard::KEY_F1));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F2")), INT2NUM(Keyboard::KEY_F2));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F3")), INT2NUM(Keyboard::KEY_F3));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F4")), INT2NUM(Keyboard::KEY_F4));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F5")), INT2NUM(Keyboard::KEY_F5));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F6")), INT2NUM(Keyboard::KEY_F6));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F7")), INT2NUM(Keyboard::KEY_F7));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F8")), INT2NUM(Keyboard::KEY_F8));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F9")), INT2NUM(Keyboard::KEY_F9));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F10")), INT2NUM(Keyboard::KEY_F10));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F11")), INT2NUM(Keyboard::KEY_F11));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F12")), INT2NUM(Keyboard::KEY_F12));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F13")), INT2NUM(Keyboard::KEY_F13));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F14")), INT2NUM(Keyboard::KEY_F14));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F15")), INT2NUM(Keyboard::KEY_F15));

    /* Key state modifier keys */
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_NUMLOCK")), INT2NUM(Keyboard::KEY_NUMLOCK));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_CAPSLOCK")), INT2NUM(Keyboard::KEY_CAPSLOCK));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_SCROLLOCK")), INT2NUM(Keyboard::KEY_SCROLLOCK));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_RSHIFT")), INT2NUM(Keyboard::KEY_RSHIFT));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_LSHIFT")), INT2NUM(Keyboard::KEY_LSHIFT));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_RCTRL")), INT2NUM(Keyboard::KEY_RCTRL));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_LCTRL")), INT2NUM(Keyboard::KEY_LCTRL));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_RALT")), INT2NUM(Keyboard::KEY_RALT));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_LALT")), INT2NUM(Keyboard::KEY_LALT));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_RMETA")), INT2NUM(Keyboard::KEY_RMETA));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_LMETA")), INT2NUM(Keyboard::KEY_LMETA));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_LSUPER")), INT2NUM(Keyboard::KEY_LSUPER));          /* Left "Windows" key */
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_RSUPER")), INT2NUM(Keyboard::KEY_RSUPER));          /* Right "Windows" key */
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_MODE")), INT2NUM(Keyboard::KEY_MODE));          /* "Alt Gr" key */
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_COMPOSE")), INT2NUM(Keyboard::KEY_COMPOSE));          /* Multi-key compose key */
}
