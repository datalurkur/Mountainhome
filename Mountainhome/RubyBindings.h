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

#define NEW_RUBY_OBJECT_FULL(bindings_type, cObj, klass) (\
    bindings_type::Get()->registerPair((cObj), Data_Wrap_Struct((klass), bindings_type::Mark, bindings_type::Free, (cObj))), \
    bindings_type::Get()->getValue((cObj)))

#define NEW_RUBY_OBJECT(bindings_type, cObj) \
    NEW_RUBY_OBJECT_FULL(bindings_type, (cObj), bindings_type::Get()->getClass())

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark RubyBindings declarations
//////////////////////////////////////////////////////////////////////////////////////////
/*! RubyBindings gives some default, static functions to help manage the connection
 *  between C++ objects and ruby objects. Typically, one of the two subclasses should be
 *  used to get more accurate memory management defaults. */
template <typename T, bool DeleteOnFree>
class RubyBindings {
public:
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

    /*! Get the static reference to the actual bindings object. */
    static RubyBindings<T, DeleteOnFree>* Get();

protected:
    static RubyBindings<T, DeleteOnFree>* Instance;
    static std::string Name;

public:
    RubyBindings(VALUE klass, const std::string &name);
    virtual ~RubyBindings();

    /*! Registers the given C++ and Ruby objects in a map, allowing access to the ruby
     *  VALUE, even when only given the C++ pointer. */
    void registerPair(T* cobj, VALUE rObj);

    /*! Removes the given C++ pointer and the associated Ruby object from the internal
     *  mapping. Raises an exception if cobj is not in the mapping. */
    void unregisterPair(T* cobj);

    /*! Extracts the pointer to the c object assoicated with the given Ruby VALUE and
     *  casts it to the proper type. */
    virtual T *getPointer(VALUE rObj);

    /*! Looks up the Ruby object associated with the given C++ object and returns its
     *  value. An exception is raised if no C++ object exists in the mapping. */
    VALUE getValue(T *cobj);

    /*! Gets the VALUE of the Ruby class associated with a particular binding. */
    VALUE getClass();

protected:
    /*! Looks for the given object in the C++ to Ruby mapping an raises an exception if
     *  the pair is not currently registered. */
    void verifyPairIsPresent(T *cobj);

private:
    /*! Don't give access to the default c'tor. Must use the exposed c'tor. */
    RubyBindings();

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
    BindingMap _cToRuby;

    /*! The ruby class associated with this binding. */
    VALUE _class;

};

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark RubyBindings static definitions
//////////////////////////////////////////////////////////////////////////////////////////
template <typename T, bool DeleteOnFree>
VALUE RubyBindings<T, DeleteOnFree>::Alloc(VALUE klass) {
    T *cObj = new T();
    VALUE rObj = Data_Wrap_Struct(klass, Mark, Free, cObj);    
    Get()->registerPair(cObj, rObj);
    return rObj;
}

template <typename T, bool DeleteOnFree>
void RubyBindings<T, DeleteOnFree>::Free(T* cobj) {
    Get()->unregisterPair(cobj);
    if (DeleteOnFree) {
        delete cobj;
    }
}

template <typename T, bool DeleteOnFree>
void RubyBindings<T, DeleteOnFree>::Mark(T* cobj) {
    // Do nothing by default.
}

template <typename T, bool DeleteOnFree>
RubyBindings<T, DeleteOnFree>* RubyBindings<T, DeleteOnFree>::Get() {
    if (!Instance) { Warn("Returning NULL instance of " << Name); }
    return Instance;
}

template <typename T, bool DeleteOnFree>
RubyBindings<T, DeleteOnFree>* RubyBindings<T, DeleteOnFree>::Instance = NULL;

template <typename T, bool DeleteOnFree>
std::string RubyBindings<T, DeleteOnFree>::Name = "[Name was never set!]";

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark RubyBindings member definitions
//////////////////////////////////////////////////////////////////////////////////////////
template <typename T, bool DeleteOnFree>
RubyBindings<T, DeleteOnFree>::RubyBindings() {
    THROW(InternalError, "[" << Name << "] Instance is already set!");
}

template <typename T, bool DeleteOnFree>
RubyBindings<T, DeleteOnFree>::RubyBindings(VALUE klass, const std::string &name)
: _class(klass)
{
    if (Instance) { THROW(InternalError, "[" << Name << "] Instance is already set!"); }
    Instance = this;
    Name = name;
}

template <typename T, bool DeleteOnFree>
RubyBindings<T, DeleteOnFree>::~RubyBindings() {}

template <typename T, bool DeleteOnFree>
T *RubyBindings<T, DeleteOnFree>::getPointer(VALUE rObj) {
    T *cObj;
    Data_Get_Struct(rObj, T, cObj);
    return cObj;
}

template <typename T, bool DeleteOnFree>
void RubyBindings<T, DeleteOnFree>::registerPair(T *cObj, VALUE rObj) {
    typename BindingMap::iterator itr = _cToRuby.find(cObj);
    int count = 1;

Info("[" << Name << "] Registering: " << cObj << "/" << rObj);

    if (itr != _cToRuby.end()) {
        if (DeleteOnFree) {
            THROW(DuplicateItemError, "[" << Name << "] Ruby object already mapped to " <<
                cObj << ". Called from " << rb_sourcefile() << ":" << rb_sourceline());
        }

        // Increment the ref count. See the ValueRef comment for an explanation.
        count = itr->second.second + 1;
    }

    _cToRuby[cObj] = ValueRef(rObj, count);
}

template <typename T, bool DeleteOnFree>
void RubyBindings<T, DeleteOnFree>::verifyPairIsPresent(T *cObj) {
    if (_cToRuby.find(cObj) == _cToRuby.end()) {
        THROW(InternalError, "[" << Name << "] Cannot find ruby object that maps to C++ object " <<
            cObj << ". Called from " << rb_sourcefile() << ":" << rb_sourceline());
    }
}

template <typename T, bool DeleteOnFree>
void RubyBindings<T, DeleteOnFree>::unregisterPair(T *cobj) {
    verifyPairIsPresent(cobj);
    ValueRef oldRef = _cToRuby[cobj];
    oldRef.second--;

    // Only remove the entry from the mapping if the ref count is zero! See the ValueRef
    // comment for an explanation.
    if (oldRef.second == 0) {
        _cToRuby.erase(cobj);
    } else {
        _cToRuby[cobj] = oldRef;
    }
}

template <typename T, bool DeleteOnFree>
VALUE RubyBindings<T, DeleteOnFree>::getValue(T *cobj) {
    verifyPairIsPresent(cobj);
    return _cToRuby[cobj].first;
}

template <typename T, bool DeleteOnFree>
VALUE RubyBindings<T, DeleteOnFree>::getClass() {
    return _class;
}

#endif
