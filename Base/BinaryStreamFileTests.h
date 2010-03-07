/*
 *  BinaryStreamFileTests.h
 *  Base
 *
 *  Created by Brent Wilson on 10/15/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _BINARYSTREAMFILETESTS_H_
#define _BINARYSTREAMFILETESTS_H_
#include "Test.h"

class BinaryStreamFileTests : public Test<BinaryStreamFileTests> {
public:
    BinaryStreamFileTests(): Test<BinaryStreamFileTests>() {}
    static void RunTests();

private:
    static void TestBinaryReadArray();
    static void TestBinaryReadOperator();
    static void TestBinaryWriteString();
    static void TestBinaryWriteArray();
    static void TestBinaryWriteOperator();
};

#endif
