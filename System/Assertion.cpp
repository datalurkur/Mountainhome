/*
 *  Assertion.cpp
 *  System
 *
 *  Created by loch on 9/20/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "Assertion.h"
#include "Math3D.h"

Assertion::Assertion(): _softAssert(false), _assertionsMade(0), _assertionsFailed(0) {}
Assertion::~Assertion() {}

bool Assertion::setSoftAbort(bool softAbort) {
    bool oldVal = _softAssert;
    _softAssert = softAbort;
    return oldVal;
}

void Assertion::getTestStats(int &made, int &failed, Real &percentage) {
    made       = _assertionsMade;
    failed     = _assertionsFailed;
    percentage = static_cast<Real>(made - failed) / static_cast<Real>(made) * 100;
    if (percentage != percentage) { percentage = 0; }
}

void Assertion::clearTestStats() {
    _assertionsMade = 0;
    _assertionsFailed = 0;
}

void Assertion::printTestStats() {
    int made, failed;
    float percent;

    getTestStats(made, failed, percent);
    Info("");
    Info("Assertions failed:  " << failed);
    Info("Assertions made:    " << made);
    Info("Assertion pass rate:" << percent << "%");
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

int Assertion::checkAssertion(
    bool value,
    const std::string &message,
    const std::string &cond,
    const std::string &file,
    int line
) {
    _assertionsMade++;
    if (value) { return 0; }
    _assertionsFailed++;

    bool abortOnError = LogStream::SetAbortOnError(false);

    Error("");
    Error("");
    Error("Assertion failed :" << file << ":" << line << ":");
    Error("Condition :" << cond);
    if (message.length()) {
        Error(message);
    }

    LogStream::SetAbortOnError(abortOnError);
    
    return _softAssert ? 1 : 2;
}

bool Assertion::feq(const Real &valueOne, const Real &valueTwo) {
    return Math::eq(valueOne, valueTwo);
}

bool Assertion::fne(const Real &valueOne, const Real &valueTwo) {
    return Math::ne(valueOne, valueTwo);
}

bool Assertion::fgt(const Real &valueOne, const Real &valueTwo) {
    return Math::gt(valueOne, valueTwo);
}

bool Assertion::fge(const Real &valueOne, const Real &valueTwo) {
    return Math::ge(valueOne, valueTwo);
}

bool Assertion::flt(const Real &valueOne, const Real &valueTwo) {
    return Math::lt(valueOne, valueTwo);
}

bool Assertion::fle(const Real &valueOne, const Real &valueTwo) {
    return Math::le(valueOne, valueTwo);
}
