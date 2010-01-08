/*
 *  TextStreamFileTest.h
 *  Base
 *
 *  Created by Brent Wilson on 10/15/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _TEXTSTREAMFILETESTS_H_
#define _TEXTSTREAMFILETESTS_H_
#include "Test.h"

class TextStreamFileTests : public Test<TextStreamFileTests> {
public:
    TextStreamFileTests(): Test<TextStreamFileTests>() {}
    static void RunTests();

private:
    static void TestTextReadAll();
    static void TestTextReadLine();
    static void TestTextReadChunk();
    static void TestTextReadOperator();
    static void TestTextWriteOperator();
};

#endif
