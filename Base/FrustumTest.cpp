//
//  FrustumTest.cpp
//  Mountainhome
//
//  Created by loch on 3/25/11.
//  Copyright 2011 Mountainhome Project. All rights reserved.
//

#include "FrustumTest.h"
#include "Frustum.h"
#include "Matrix.h"

void TestFrustum::RunTests() {
    TestOrthoDecomp();
    TestPerspectiveDecomp();
}

void TestFrustum::TestOrthoDecomp() {
    Matrix mat = Matrix::Ortho(-1, 1, -1, 1, -1, 1);

    Frustum frustum;
    frustum.setProjectionMatrix(mat);

    TASSERT_EQ(Plane(Vector3( 1,  0,  0), -1), *(frustum.getPlane(Frustum::LEFT)));
    TASSERT_EQ(Plane(Vector3(-1,  0,  0), -1), *(frustum.getPlane(Frustum::RIGHT)));
    TASSERT_EQ(Plane(Vector3( 0,  1,  0), -1), *(frustum.getPlane(Frustum::BOTTOM)));
    TASSERT_EQ(Plane(Vector3( 0, -1,  0), -1), *(frustum.getPlane(Frustum::TOP)));
    TASSERT_EQ(Plane(Vector3( 0,  0, -1), -1), *(frustum.getPlane(Frustum::NEAR)));
    TASSERT_EQ(Plane(Vector3( 0,  0,  1), -1), *(frustum.getPlane(Frustum::FAR)));

    frustum.setWorldMatrix(Matrix::Translation(Vector3(1, 1, 1)).getInverse());

    TASSERT_EQ(Plane(Vector3( 1,  0,  0), -0), *(frustum.getPlane(Frustum::LEFT)));
    TASSERT_EQ(Plane(Vector3(-1,  0,  0), -2), *(frustum.getPlane(Frustum::RIGHT)));
    TASSERT_EQ(Plane(Vector3( 0,  1,  0), -0), *(frustum.getPlane(Frustum::BOTTOM)));
    TASSERT_EQ(Plane(Vector3( 0, -1,  0), -2), *(frustum.getPlane(Frustum::TOP)));
    TASSERT_EQ(Plane(Vector3( 0,  0, -1), -0), *(frustum.getPlane(Frustum::NEAR)));
    TASSERT_EQ(Plane(Vector3( 0,  0,  1), -2), *(frustum.getPlane(Frustum::FAR)));
}

void TestFrustum::TestPerspectiveDecomp() {
    Matrix mat = Matrix::Perspective(1.0, Degree(45), 1.0, 10000.0);

    Frustum frustum;
    frustum.setProjectionMatrix(mat);

//    TASSERT(frustum.getPlane(Frustum::LEFT  )->getDistance() < 0);
//    TASSERT(frustum.getPlane(Frustum::RIGHT )->getDistance() < 0);
//    TASSERT(frustum.getPlane(Frustum::BOTTOM)->getDistance() < 0);
//    TASSERT(frustum.getPlane(Frustum::TOP   )->getDistance() < 0);
//    TASSERT(frustum.getPlane(Frustum::NEAR  )->getDistance() < 0);
//    TASSERT(frustum.getPlane(Frustum::FAR   )->getDistance() < 0);
}
