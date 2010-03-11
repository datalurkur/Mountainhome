/*
 *  RubyKeyboard.cpp
 *  Mountainhome
 *
 *  Created by Paul on 3/8/10.
 *  Copyright 2010 MHCore Project. All rights reserved.
 *
 */

#include "RubyKeyboard.h"
#include <Base/Logger.h>

VALUE RubyKeyboard::Class = NULL;
VALUE RubyKeyboard::RubyLookup = NULL;

// For some reason name is a String here, rather than a Symbol.
VALUE RubyKeyboard::Method_Missing(int argc, VALUE * argv, VALUE self) {
    VALUE k = argv[0];
    VALUE v = rb_hash_aref(RubyLookup, k);
//    printf("name: %lu\n", name);
//    Info(rb_string_value_cstr(&name));
    Info((rb_obj_is_instance_of(k, rb_cSymbol) == Qtrue));
    Info((rb_obj_is_instance_of(v, rb_cFixnum) == Qtrue));
    
//    VALUE nameclass = rb_funcall(name, rb_intern("class"), 0);
//    VALUE nameclassname = rb_funcall(nameclass, rb_intern("to_s"), 0);
//    Info(rb_string_value_cstr(&nameclassname));
//    
//    ID namesym = rb_funcall(name, rb_intern("to_sym"), 0);
//    VALUE nameint = rb_funcall(name, rb_intern("to_i"), 0);
//    VALUE ret = rb_hash_aref(RubyLookup, namesym);
    if(v != Qnil) {
        return v;
    } else {
        Info("Before calling super...");
        return rb_call_super(argc, argv);
    }
}

VALUE RubyKeyboard::Map_Key(VALUE key, VALUE value, VALUE self) {
    rb_hash_aset(RubyLookup, key, value);
    return Qnil;
}

void RubyKeyboard::SetupBindings() {
    Class = rb_define_class("Keyboard", rb_cObject);
    rb_define_singleton_method(Class, "method_missing",  RUBY_METHOD_FUNC(RubyKeyboard::Method_Missing),  -1);
    rb_define_singleton_method(Class, "map_key",  RUBY_METHOD_FUNC(RubyKeyboard::Map_Key),  2);
    rb_define_class_variable(Class, "@@lookup", rb_hash_new());
    RubyLookup = rb_cv_get(Class, "@@lookup");

    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_UNKNOWN")), INT2NUM(0));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_FIRST")), INT2NUM(0));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_BACKSPACE")), INT2NUM(8));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_TAB")), INT2NUM(9));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_CLEAR")), INT2NUM(12));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_RETURN")), INT2NUM(13));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_PAUSE")), INT2NUM(19));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_ESCAPE")), INT2NUM(27));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_SPACE")), INT2NUM(32));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_EXCLAIM")), INT2NUM(33));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_QUOTEDBL")), INT2NUM(34));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_HASH")), INT2NUM(35));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_DOLLAR")), INT2NUM(36));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_AMPERSAND")), INT2NUM(38));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_QUOTE")), INT2NUM(39));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_LEFTPAREN")), INT2NUM(40));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_RIGHTPAREN")), INT2NUM(41));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_ASTERISK")), INT2NUM(42));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_PLUS")), INT2NUM(43));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_COMMA")), INT2NUM(44));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_MINUS")), INT2NUM(45));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_PERIOD")), INT2NUM(46));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_SLASH")), INT2NUM(47));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_0")), INT2NUM(48));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_1")), INT2NUM(49));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_2")), INT2NUM(50));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_3")), INT2NUM(51));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_4")), INT2NUM(52));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_5")), INT2NUM(53));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_6")), INT2NUM(54));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_7")), INT2NUM(55));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_8")), INT2NUM(56));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_9")), INT2NUM(57));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_COLON")), INT2NUM(58));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_SEMICOLON")), INT2NUM(59));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_LESS")), INT2NUM(60));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_EQUALS")), INT2NUM(61));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_GREATER")), INT2NUM(62));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_QUESTION")), INT2NUM(63));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_AT")), INT2NUM(64));
    /* 
    Skip uppercase letters
    */
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_LEFTBRACKET")), INT2NUM(91));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_BACKSLASH")), INT2NUM(92));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_RIGHTBRACKET")), INT2NUM(93));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_CARET")), INT2NUM(94));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_UNDERSCORE")), INT2NUM(95));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_BACKQUOTE")), INT2NUM(96));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_a")), INT2NUM(97));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_b")), INT2NUM(98));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_c")), INT2NUM(99));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_d")), INT2NUM(100));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_e")), INT2NUM(101));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_f")), INT2NUM(102));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_g")), INT2NUM(103));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_h")), INT2NUM(104));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_i")), INT2NUM(105));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_j")), INT2NUM(106));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_k")), INT2NUM(107));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_l")), INT2NUM(108));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_m")), INT2NUM(109));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_n")), INT2NUM(110));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_o")), INT2NUM(111));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_p")), INT2NUM(112));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_q")), INT2NUM(113));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_r")), INT2NUM(114));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_s")), INT2NUM(115));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_t")), INT2NUM(116));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_u")), INT2NUM(117));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_v")), INT2NUM(118));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_w")), INT2NUM(119));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_x")), INT2NUM(120));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_y")), INT2NUM(121));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_z")), INT2NUM(122));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_DELETE")), INT2NUM(127));
    /* End of ASCII mapped keysyms */

    /* Numeric keypad */
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP0")), INT2NUM(256));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP1")), INT2NUM(257));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP2")), INT2NUM(258));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP3")), INT2NUM(259));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP4")), INT2NUM(260));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP5")), INT2NUM(261));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP6")), INT2NUM(262));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP7")), INT2NUM(263));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP8")), INT2NUM(264));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP9")), INT2NUM(265));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP_PERIOD")), INT2NUM(266));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP_DIVIDE")), INT2NUM(267));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP_MULTIPLY")), INT2NUM(268));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP_MINUS")), INT2NUM(269));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP_PLUS")), INT2NUM(270));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP_ENTER")), INT2NUM(271));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_KP_EQUALS")), INT2NUM(272));

    /* Arrows + Home/End pad */
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_UP")), INT2NUM(273));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_DOWN")), INT2NUM(274));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_RIGHT")), INT2NUM(275));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_LEFT")), INT2NUM(276));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_INSERT")), INT2NUM(277));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_HOME")), INT2NUM(278));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_END")), INT2NUM(279));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_PAGEUP")), INT2NUM(280));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_PAGEDOWN")), INT2NUM(281));

    /* Function keys */
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F1")), INT2NUM(282));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F2")), INT2NUM(283));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F3")), INT2NUM(284));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F4")), INT2NUM(285));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F5")), INT2NUM(286));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F6")), INT2NUM(287));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F7")), INT2NUM(288));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F8")), INT2NUM(289));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F9")), INT2NUM(290));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F10")), INT2NUM(291));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F11")), INT2NUM(292));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F12")), INT2NUM(293));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F13")), INT2NUM(294));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F14")), INT2NUM(295));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_F15")), INT2NUM(296));

    /* Key state modifier keys */
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_NUMLOCK")), INT2NUM(300));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_CAPSLOCK")), INT2NUM(301));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_SCROLLOCK")), INT2NUM(302));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_RSHIFT")), INT2NUM(303));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_LSHIFT")), INT2NUM(304));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_RCTRL")), INT2NUM(305));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_LCTRL")), INT2NUM(306));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_RALT")), INT2NUM(307));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_LALT")), INT2NUM(308));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_RMETA")), INT2NUM(309));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_LMETA")), INT2NUM(310));
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_LSUPER")), INT2NUM(311));          /* Left "Windows" key */
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_RSUPER")), INT2NUM(312));          /* Right "Windows" key */
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_MODE")), INT2NUM(313));          /* "Alt Gr" key */
    rb_hash_aset(RubyLookup, ID2SYM(rb_intern("KEY_COMPOSE")), INT2NUM(314));          /* Multi-key compose key */
}
