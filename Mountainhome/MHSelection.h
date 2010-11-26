/*
 *  MHSelection.h
 *  Mountainhome
 *
 *  Created by datalurkur on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHSELECTION_H_
#define _MHSELECTION_H_

#include "MHActor.h"
#include "Vector.h"

template <class T>
class TemplatedSelection {
public:
    TemplatedSelection();
    virtual ~TemplatedSelection();

    void clear();
    void append(T obj);
    int size();
    std::list <T> &getSelected();

private:
    std::list <T> _selected;
};

template <class T>
TemplatedSelection<T>::TemplatedSelection() {}

template <class T>
TemplatedSelection<T>::~TemplatedSelection() {}

template <class T>
void TemplatedSelection<T>::clear() {
    _selected.clear();
}

template <class T>
void TemplatedSelection<T>::append(T obj) {
    _selected.push_back(obj);
}

template <class T>
int TemplatedSelection<T>::size() {
    return _selected.size();
}

template <class T>
std::list <T> &TemplatedSelection<T>::getSelected() {
    return _selected;
}

class MHActorSelection: public TemplatedSelection<MHActor*> {};
class MHTileSelection: public TemplatedSelection<Vector3> {};

#endif
