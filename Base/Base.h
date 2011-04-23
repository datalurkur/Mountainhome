/*
 *  Base.h
 *  Base
 *
 *  Created by Brent Wilson on 9/3/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _BASE_H_
#define _BASE_H_

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>

typedef float Real;

#include "Exception.h"
#include "Platform.h"
#include "Logger.h"

//////////////////////////////
// Generic helper functions //
//////////////////////////////
#if SYS_PLATFORM == PLATFORM_APPLE
#include <Carbon/Carbon.h>
inline std::string macBundlePath() {
    char path[1024];
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    assert(mainBundle);

    CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
    assert(mainBundleURL);

    CFStringRef cfStringRef = CFURLCopyFileSystemPath(mainBundleURL, kCFURLPOSIXPathStyle);
    assert(cfStringRef);

    CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);

    CFRelease(mainBundleURL);
    CFRelease(cfStringRef);

    return std::string(path);
}
#endif

template <typename T>
inline void tokenize(const std::string &input, const std::string &delim,
T &tokens) {
    size_t start = input.find_first_not_of(delim);
    size_t end = input.find_first_of(delim, start);
    while(start != std::string::npos) {
        tokens.push_back(input.substr(start, end - start));
        start = input.find_first_not_of(delim, end);
        end = input.find_first_of(delim, start);
    }
}

template <typename T>
inline void clear_map(T &map) {
    typename T::iterator itr = map.begin();
    for (; itr != map.end(); itr++) {
        if (itr->second) { delete itr->second; }
    }

    map.clear();
}

template <typename T>
inline void clear_list(T &list) {
    typename T::iterator itr = list.begin();
    for (; itr != list.end(); itr++) {
        if (*itr) { delete *itr; }
    }

    list.clear();
}

template <typename T>
inline T* vector_to_array(std::vector<T> &vec) {
    unsigned int size_to_copy = sizeof(T)*vec.size();
    T *ary = (T*)malloc(size_to_copy);
    memcpy(ary, &(vec[0]), size_to_copy);
    return ary;
}


template <typename T>
inline std::string to_s(T value) {
   	std::ostringstream stream;
	stream << value;
	return stream.str();
}

inline float to_f(const std::string &value) {
    float level;
    sscanf(value.c_str(), "%f", &level);
	return level;
}

inline int to_i(const std::string &value) {
    int level;
    sscanf(value.c_str(), "%i", &level);
	return level;
}

inline int to_pot(int v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    ++v;
    return v;
}

////////////////////////////
// Compile time functions //
////////////////////////////
template <bool val>
struct bool_type {
    static const bool value = val;
};

// has_type
template <typename T1, typename T2>
struct has_type : public bool_type<false> {};

template <> struct has_type<void,               void              > : public bool_type<true> {};
template <> struct has_type<float,              float             > : public bool_type<true> {};
template <> struct has_type<double,             double            > : public bool_type<true> {};
template <> struct has_type<char,               char              > : public bool_type<true> {};
template <> struct has_type<unsigned char,      unsigned char     > : public bool_type<true> {};
template <> struct has_type<short,              short             > : public bool_type<true> {};
template <> struct has_type<unsigned short,     unsigned short    > : public bool_type<true> {};
template <> struct has_type<int,                int               > : public bool_type<true> {};
template <> struct has_type<unsigned int,       unsigned int      > : public bool_type<true> {};
template <> struct has_type<long,               long              > : public bool_type<true> {};
template <> struct has_type<unsigned long,      unsigned long     > : public bool_type<true> {};
template <> struct has_type<long long,          long long         > : public bool_type<true> {};
template <> struct has_type<unsigned long long, unsigned long long> : public bool_type<true> {};


// is_float
template <typename T>
struct is_float : public bool_type<false> {};
template <> struct is_float<float> : public bool_type<true> {};
template <> struct is_float<double> : public bool_type<true> {};

template <typename T>
struct is_double : public bool_type<false> {};
template <> struct is_double<double> : public bool_type<true> {};

template <typename T>
struct is_single : public bool_type<false> {};
template <> struct is_single<float> : public bool_type<true> {};

// is_numeric
template <typename T>
struct is_numeric : public bool_type<false> {};

template <> struct is_numeric<char               > : public bool_type<true> {};
template <> struct is_numeric<unsigned char      > : public bool_type<true> {};
template <> struct is_numeric<short              > : public bool_type<true> {};
template <> struct is_numeric<unsigned short     > : public bool_type<true> {};
template <> struct is_numeric<int                > : public bool_type<true> {};
template <> struct is_numeric<unsigned int       > : public bool_type<true> {};
template <> struct is_numeric<long               > : public bool_type<true> {};
template <> struct is_numeric<unsigned long      > : public bool_type<true> {};
template <> struct is_numeric<long long          > : public bool_type<true> {};
template <> struct is_numeric<unsigned long long > : public bool_type<true> {};
template <> struct is_numeric<float              > : public bool_type<true> {};
template <> struct is_numeric<double             > : public bool_type<true> {};

#endif
