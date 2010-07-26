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
#include <Base/Assertion.h>
#include <Base/Singleton.h>
#include <Engine/Engine.h>
#include <utility>

///\todo XXXBMW: Wtf hack.... Compile fails if I don't do this. Clearly not how it's
// supposed to work, though. I'll have to look into this later.
#define HAVE_STRUCT_TIMESPEC 1
#include <ruby.h>

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Helper functions
//////////////////////////////////////////////////////////////////////////////////////////
VALUE require_setup_wrapper(VALUE arg);
VALUE get_class_value(const char *name);
void translate_ruby_exception(int error);

#define CreateBindingPairWithClass(klass, type, cObj) (\
    type::RegisterPair(cObj, Data_Wrap_Struct(klass, type::Mark, type::Free, cObj)), \
    type::GetValue(cObj))

#define CreateBindingPair(type, cObj) \
    CreateBindingPairWithClass(type::GetClass(), type, cObj)

#define AssignCObjFromValue(type, cObj, rObj) \
    type *cObj; Data_Get_Struct(rObj, type, cObj);

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark RubyBindings declarations
//////////////////////////////////////////////////////////////////////////////////////////
/*! RubyBindings gives some default, static functions to help manage the connection
 *  between C++ objects and ruby objects. Typically, one of the two subclasses should be
 *  used to get more accurate memory management defaults.
 * \seealso RubyCreatedObjectBindings
 * \seealso CPPCreatedObjectBindings */
template <typename T, bool DeleteOnFree>
class RubyBindings {
public:
    /*! Registers the given C++ and Ruby objects in a map, allowing access to the ruby
     *  VALUE, even when only given the C++ pointer. */
    static void RegisterPair(T* cobj, VALUE robj);

    /*! Removes the given C++ pointer and the associated Ruby object from the internal
     *  mapping. Raises an exception if cobj is not in the mapping. */
    static void UnregisterPair(T* cobj);

    /*! Looks up the Ruby object associated with the given C++ object and returns its
     *  value. An exception is raised if no C++ object exists in the mapping. */
    static VALUE GetValue(T *cobj);

    /*! Gets the VALUE of the Ruby class associated with a particular binding. */
    static VALUE GetClass();

    /*! This default implementation creates a new C++ object of type T and attaches it to
     *  the data slot of a new Ruby object, then registers the pair with the mapping. */
    static VALUE Alloc(VALUE klass);

    /*! Ruby objects created by these bindings provide two Ruby Garbage Collector
     *  callbacks: Mark and Free. This callback allows bindings to call 'rb_gc_mark' on
     *  ruby VALUEs associated with them, making sure the ruby GC will not delete them.
     *  This default implementation does nothing, but may be overridden by subclasses. */
    static void Mark(T* cobj);

    /*! This default implementation removes the object mappings and, if DeleteOnFree is
     *  true, deletes the given object. */
    static void Free(T* cobj);

protected:
    /*! Looks for the given object in the C++ to Ruby mapping an raises an exception if
     *  the pair is not currently registered. */
    static void VerifyPairIsPresent(T *cobj);

protected:
    /*! When the c memory is handled by c, it is entirely possible for a chunk of memory
     *  to be freed and reallocated for the exact same time of object, then registered
     *  here before the ruby GC has run to unregister the pair from the bindings. When
     *  that old ruby object is GCed, it will nuke the new pair, which is bad. This is why
     *  we implement ref counting for pairs. Technically it shouldn't be needed for
     *  DeleteOnFree relationships as the c memory should only become available AFTER the
     *  ruby object is GCed, but we leave it for both cases. */
    typedef std::pair<VALUE, int> ValueRef;

    /*! The type that maps c memory to a ruby VALUE. */
    typedef std::map<T*, ValueRef> BindingMap;

    /*! Maintains the c => ruby mapping, used to lookup ruby values from c. The reverse
     *  mapping is handled by the ruby API */
    static BindingMap CToRuby;

    /*! The ruby class associated with this binding. */
    static VALUE Class;

};

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark RubyBindings definitions
//////////////////////////////////////////////////////////////////////////////////////////
template <typename T, bool DeleteOnFree>
typename RubyBindings<T, DeleteOnFree>::BindingMap RubyBindings<T, DeleteOnFree>::CToRuby;

template <typename T, bool DeleteOnFree>
VALUE RubyBindings<T, DeleteOnFree>::Class;

template <typename T, bool DeleteOnFree>
void RubyBindings<T, DeleteOnFree>::RegisterPair(T* cobj, VALUE robj) {
    typename BindingMap::iterator itr = CToRuby.find(cobj);
    int count = 1;

    if (itr != CToRuby.end()) {
        if (DeleteOnFree) {
            THROW(DuplicateItemError, "Ruby object already mapped to " << cobj <<
                ". Called from " << rb_sourcefile() << ":" << rb_sourceline());
        }

        // Increment the ref count. See the ValueRef comment for an explanation.
        count = itr->second.second + 1;
    }

    CToRuby[cobj] = ValueRef(robj, count);
}

template <typename T, bool DeleteOnFree>
void RubyBindings<T, DeleteOnFree>::VerifyPairIsPresent(T *cobj) {
    if (CToRuby.find(cobj) == CToRuby.end()) {
        THROW(InternalError, "Cannot find ruby object that maps to C++ object " << cobj <<
            ". Called from " << rb_sourcefile() << ":" << rb_sourceline());
    }
}

template <typename T, bool DeleteOnFree>
void RubyBindings<T, DeleteOnFree>::UnregisterPair(T* cobj) {
    VerifyPairIsPresent(cobj);
    ValueRef oldRef = CToRuby[cobj];
    oldRef.second--;

    // Only remove the entry from the mapping if the ref count is zero! See the ValueRef
    // comment for an explanation.
    if (oldRef.second == 0) {
        CToRuby.erase(cobj);
    } else {
        CToRuby[cobj] = oldRef;
    }
}

template <typename T, bool DeleteOnFree>
VALUE RubyBindings<T, DeleteOnFree>::GetValue(T *cobj) {
    VerifyPairIsPresent(cobj);
    return CToRuby[cobj].first;
}

template <typename T, bool DeleteOnFree>
VALUE RubyBindings<T, DeleteOnFree>::GetClass() {
    return Class;
}

template <typename T, bool DeleteOnFree>
VALUE RubyBindings<T, DeleteOnFree>::Alloc(VALUE klass) {
    T *cobj = new T();
    VALUE robj = CreateBindingPairWithClass(klass, T, cobj);
    return robj;
}

template <typename T, bool DeleteOnFree>
void RubyBindings<T, DeleteOnFree>::Free(T* cobj) {
    UnregisterPair(cobj);
    if (DeleteOnFree) {
        delete cobj;
    }
}

template <typename T, bool DeleteOnFree>
void RubyBindings<T, DeleteOnFree>::Mark(T* cobj) {
    // Do nothing by default.
}

#endif
