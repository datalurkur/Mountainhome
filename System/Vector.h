/*
 *  Vector.h
 *  SceneGraph
 *
 *  Created by Brent Wilson on 11/8/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _VECTOR_H_
#define _VECTOR_H_
#include "VectorBase.h"

template <int N>
class Vector : public VectorBase<Vector<N>, N> {};

template <int N>
std::ostream& operator<<(std::ostream &lhs, const Vector<N> &rhs) {
    lhs << "Vector" << N << " ( ";
    for (int i = 0; i < N - 1; i++) {
        lhs << rhs[i] << ", ";
    }

    lhs << rhs[N - 1] << " )";
    return lhs;
}

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#endif
