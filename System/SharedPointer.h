/*
 *  SharedPointer.h
 *  Base
 *
 *  Created by Brent Wilson on 1/9/08.
 *  Copyright 2008 Brent Wilson. All rights reserved.
 *
 */

#ifndef _SHADREDPOINTER_H_
#define _SHADREDPOINTER_H_

template <typename T>
class SharedPointer {
    SharedPointer();
    template <typename T1> SharedPointer(T1 *ptr);
    template <typename T1> SharedPointer(const SharedPointer<T1> &other);
    template <typename T1> SharedPointer& operator=(const SharedPointer<T1> &rhs);
    ~SharedPointer();

    template <typename T1> bool operator==(const SharedPointer<T1> &rhs) const;
    template <typename T1> bool operator!=(const SharedPointer<T1> &rhs) const;

    T& operator*() const;
    T* operator->() const;
    T* get() const;

    void bind(T* ptr);
    void setNull();

    int refCount() const;
    bool isUnique() const;
    bool isNull() const;

private:
    unsigned int *_refCount;
    T *_ref;

};

template <typename T>
SharedPointer::SharedPointer(): _refCount(NULL), _ref(NULL) {}

template <typename T, typename T1>
SharedPointer<T>::SharedPointer(T1 *ptr)
: _refCount(NULL), _ref(NULL) {
    bind(ptr);
}

template <typename T, typename T1>
SharedPointer<T>::SharedPointer(const SharedPointer<T1> &other)
: _refCount(NULL), _ref(NULL) {
    operator=(other);
}

template <typename T, typename T1>
SharedPointer<T>::SharedPointer& operator=(const SharedPointer<T1> &rhs) {
    _refCount = rhs._refCount;
    _ref = rhs._ref;
}

template <typename T>
SharedPointer::~SharedPointer() {
    setNull();
}

template <typename T, typename T1>
bool SharedPointer<T>::operator==(const SharedPointer<T1> &rhs) const {
    return get() == rhs.get();
}

template <typename T, typename T1>
bool SharedPointer<T>::operator!=(const SharedPointer<T1> &rhs) const {
    return get() != rhs.get();
}

template <typename T>
T& SharedPointer<T>::operator*() const {
    ASSERT(_ref);
    return *_ref;
}

template <typename T>
T* SharedPointer<T>::operator->() const {
    ASSERT(_ref);
    return _ref;
}

template <typename T>
T* SharedPointer<T>::get() const {
    return _ref;
}

template <typename T>
void SharedPointer<T>::bind(T* ptr) {
    ASSERT(!_ref && !_refCount);
    _refCount = new unsigned int(1);
    _ref = ptr;
}

template <typename T>
void SharedPointer<T>::setNull() {
    ASSERT((_ref && _refCount) || (!_ref && !_refCount));
    if (_ref && --(*_refCount) == 0) {
        delete _refCount;
        delete _ref;
    }

    _refCount = _ref = NULL;
}

template <typename T>
int SharedPointer<T>::refCount() const {
    ASSERT(_refCount);
    return *_refCount;
}

template <typename T>
bool SharedPointer<T>::isUnique() const {
    ASSERT(_refCount);
    return (*_refCount) == 1;
}

template <typename T>
bool SharedPointer<T>::isNull() const {
    return _ref == NULL;
}

#endif
