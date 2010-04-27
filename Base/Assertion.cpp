/*
 *  Assertion.cpp
 *  Base
 *
 *  Created by Brent Wilson on 9/20/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "Assertion.h"
#include "Math3D.h"

#if SYS_PLATFORM == PLATFORM_APPLE
#   include <Carbon/Carbon.h>
#endif

void SoftAssert() {
#if SYS_PLATFORM == PLATFORM_APPLE
    Debugger();
#else
    __asm__("int $03");
#endif    
}

AssertionHandler::AssertionHandler(): _softAssert(true), _assertionsMade(0), _assertionsFailed(0) {}
AssertionHandler::~AssertionHandler() {}

bool AssertionHandler::setSoftAbort(bool softAbort) {
    bool oldVal = _softAssert;
    _softAssert = softAbort;
    return oldVal;
}

void AssertionHandler::getTestStats(int &made, int &failed, Real &percentage) {
    made       = _assertionsMade;
    failed     = _assertionsFailed;
    percentage = static_cast<Real>(made - failed) / static_cast<Real>(made) * 100;
    if (percentage != percentage) { percentage = 0; }
}

void AssertionHandler::clearTestStats() {
    _assertionsMade = 0;
    _assertionsFailed = 0;
}

void AssertionHandler::printTestStats() {
    int made, failed;
    float percent;

    getTestStats(made, failed, percent);
    Info("");
    Info("Assertions failed:   " << failed);
    Info("Assertions made:     " << made);
    Info("Assertion pass rate: " << percent << "%");
    if (!made) {
        return;
    }

    std::string bar = "[\e[32m"; // Set color to green

    int barLength = 50;
    int greenChunks = percent * barLength / 100.0;
    for (int i = 0; i < barLength; i++) {
        if (i == greenChunks) {
            bar += "\e[31m"; // Set color to red
        }

        bar += "=";
    }

    bar += "\e[0m]";// Reset the color
    Info(bar); 
    Info("");
}

int AssertionHandler::checkAssertion(
    bool value,
    const char *message,
    const char *cond,
    const char *file,
    int line
) {
    _assertionsMade++;
    if (value) { return 0; }
    _assertionsFailed++;

    bool abortOnError = LogStream::SetBreakOnError(false);

    Error("");
    Error("Assertion failed - " << file << ":" << line);
    Error("Condition - " << cond);
    if (std::string(message).length()) {
        Error(message);
    }
    Error("");

    LogStream::SetBreakOnError(abortOnError);

    return _softAssert ? 1 : 2;
}

bool AssertionHandler::feq(const Real &valueOne, const Real &valueTwo) {
    return Math::eq(valueOne, valueTwo);
}

bool AssertionHandler::fne(const Real &valueOne, const Real &valueTwo) {
    return Math::ne(valueOne, valueTwo);
}

bool AssertionHandler::fgt(const Real &valueOne, const Real &valueTwo) {
    return Math::gt(valueOne, valueTwo);
}

bool AssertionHandler::fge(const Real &valueOne, const Real &valueTwo) {
    return Math::ge(valueOne, valueTwo);
}

bool AssertionHandler::flt(const Real &valueOne, const Real &valueTwo) {
    return Math::lt(valueOne, valueTwo);
}

bool AssertionHandler::fle(const Real &valueOne, const Real &valueTwo) {
    return Math::le(valueOne, valueTwo);
}
