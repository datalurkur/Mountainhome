/*
 *  RubyBindings.h
 *  System
 *
 *  Created by loch on 2/15/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _RUBYBINDINGS_H_
#define _RUBYBINDINGS_H_
#include <Base/Singleton.h>
#include <Engine/Engine.h>

///\todo XXXBMW: Wtf hack.... Compile fails if I don't do this. Clearly not how it's
// supposed to work, though. I'll have to look into this later.
#define HAVE_STRUCT_TIMESPEC 1
#include <ruby.h>

template <typename T>
class ManyObjectBinding {
public:
    static void RegisterObject(VALUE robj, T* cobj);
    static T* GetObject(VALUE robj);

protected:
    static std::map<VALUE, T*> Objects;
    static VALUE Class;

};

template <typename T> std::map<VALUE, T*> ManyObjectBinding<T>::Objects;
template <typename T> VALUE ManyObjectBinding<T>::Class;

template <typename T>
void ManyObjectBinding<T>::RegisterObject(VALUE robj, T* cobj) {
    if (Objects.find(robj) != Objects.end()) {
        THROW(DuplicateItemError, "Object already exists for " << robj << "!");
    }

    Objects[robj] = cobj;
}

template <typename T>
T* ManyObjectBinding<T>::GetObject(VALUE robj) {
    if (Objects.find(robj) == Objects.end()) {
        THROW(InternalError, "Object does not exist for " << robj << "!");
    }

    return Objects[robj];
}

#endif
