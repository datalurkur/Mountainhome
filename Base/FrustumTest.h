//
//  FrustumTest.h
//  Mountainhome
//
//  Created by loch on 3/25/11.
//  Copyright 2011 Mountainhome Project. All rights reserved.
//

#ifndef _TESTMATRIX_H_
#define _TESTMATRIX_H_
#include "Test.h"

class TestFrustum : public Test<TestFrustum> {
public:
    TestFrustum(): Test<TestFrustum>() {}
    static void RunTests();

private:
    static void TestOrthoDecomp();
    static void TestPerspectiveDecomp();

};

#endif
