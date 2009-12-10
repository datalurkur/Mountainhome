/*
 *  Singleton.h
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SINGLETON_H_
#define _SINGLETON_H_
#include <memory>

/*! This is a templated class to aid in the creation of singleton classes. Singleton
 *  classes are useful, but should be used sparingly. By extending this class you are
 *  effectively guarenteeing that only one instance of the subclass can ever exist in the
 *  system at one given time. The singleton may be accessed using the 'GetInstance'
 *  method.
 *
 *  When creating a singleton class, the class must pass itself as a template arguement to
 *  Singleton. The singleton class should also make its contructors protected to prevent
 *  additional copies of the class being created. To do this, however, you must make
 *  Singleton a friend of the new class (because protected only gives access to children).
 *  Here is a quick example of a singleton class:
 *
 *  class A : public Singleton<A> {
 *  public:
 *      ...
 *
 *  protected:
 *      A();
 *      virtual ~A();
 *
 *      template <class T> friend class Singleton;
 *  };
 *
 *  \brief A templated class meant to aid in the creation of singltons.
 *  \note If a dynamic library creates and uses a singleton, and then a program using said
 *        library tries to also use the same singleton, THEY WILL BE DIFFERENT OBJECTS.
 *        static things (like the singleton) are loaded at program load and library load.
 *        In the case where a program is using a library, there will be TWO instances of
 *        the singleton. This may be resolvable by doing a static link.
 *  \author Brent Wilson
 *  \date 4/22/07 */
template <typename T>
class Singleton {
protected:
    /*! Empty default contructor */
    Singleton() {}

public:
    /*! This method has been left as public to ensure the Singleton object can be deleted
     *  properly. This method should never directly be called by the user. */
    virtual ~Singleton() {}

    /*! This is the method used to access the singleton instance
     *  \return A pointer to the singleton instance. */
    static T* GetSingleton() {
        if (_instance.get() == NULL) {
            _instance.reset(new T());
        }

        return dynamic_cast<T*>(_instance.get());
    }

protected:
    /*! An auto pointer is used to allow automatic deletion of of the instance. A pointer
     *  was used to prevent automatic creation of all singletons at program load. If this
     *  was the case, certain things could not be singletons (i.e. Anything with a
     *  contructor that relies on outside state, such as an OpenGL context). An auto_ptr
     *  was chosen to remove the need for manual deletion of the singleton object. */
    static std::auto_ptr<Singleton> _instance;

};

template <typename T>
std::auto_ptr<Singleton<T> > Singleton<T>::_instance;

#endif
