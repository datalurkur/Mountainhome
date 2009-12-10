/*
 *  System.h
 *  System
 *
 *  Created by loch on 9/3/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

typedef float Real;

template <bool val>
struct bool_type {
    static const bool value = val;
};

// is_float
template <typename T>
struct is_float : public bool_type<false> {};

template <> struct is_float<float> : public bool_type<true> {};
template <> struct is_float<double> : public bool_type<true> {};


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
