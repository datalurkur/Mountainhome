/*
 *  Test.cpp
 *  System
 *
 *  Created by loch on 10/15/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "Test.h"

TestRunner::TestList *TestRunner::_tests = NULL;

void TestRunner::init() {
    if (!_tests) {
        _tests = new TestList();
        atexit(deinit);
    }
}

void TestRunner::deinit() {
    if (_tests) {
        delete _tests;
        _tests = 0;
    }
}

void TestRunner::RunAllTests() {
    init();
    bool softAbort = Assertion::GetSingleton()->setSoftAbort(true);
    for (TestList::iterator i = _tests->begin(); i != _tests->end(); i++) {
        (*i)();
    }

    Assertion::GetSingleton()->printTestStats();
    Assertion::GetSingleton()->clearTestStats();
    Assertion::GetSingleton()->setSoftAbort(softAbort);
}

void TestRunner::AddTest(FunctionPtr func) {
    init();
    _tests->push_back(func);
}

void TestRunner::RemoveTest(FunctionPtr func) {
    if (!_tests) { return; } // Don't worry about it if _tests is null.
    _tests->remove(func);
}
