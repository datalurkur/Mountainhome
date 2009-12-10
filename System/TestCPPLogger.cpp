/*
 *  TestCPPLogger.cpp
 *  System
 *
 *  Created by loch on 10/22/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "TestCPPLogger.h"

// TODO Why am I using regular assert instead of my ASSERT macros? I think it had to do with dependencies, but I don't remember.
#include <assert.h>

void TestCPPLogger::RunTests() {
    Debug("Testing LogStream...");

    // Store the old information. If we don't we may overwrite user set values since the
    // user may have setup logging parameters before calling the tests.
    std::string oldTarget(LogStream::Logfile);
    std::string oldPretext(LogStream::Pretext);
    LogStream::LogType oldLevel(LogStream::LogLevel);
    bool abortOnError(LogStream::AbortOnError);
    int indentLevel(LogStream::IndentLevel);
    int indentSize(LogStream::IndentSize);
    bool trim(LogStream::TrimFilenames);

    // Set a bogus log file to use for 
    LogStream::SetAbortOnError(false);
    LogStream::SetLogTarget("az84Nj30kf.log");
    LogStream::SetLogLevel(LogStream::InfoMessage);
    LogStream::SetIndentLevel(0);
    LogStream::SetIndentSize(2);
    LogStream::SetPretext("");
    LogStream::SetTrimFilenames(false);

    TestTags();
    TestIndentLevel();
    TestPretext();
    TestLogTarget();
    TestLogLevel();
    TestClearFile();

    // Restore the old static parameters
    LogStream::SetAbortOnError(abortOnError);
    LogStream::SetLogTarget(oldTarget);
    LogStream::SetPretext(oldPretext);
    LogStream::SetLogLevel(oldLevel);
    LogStream::SetIndentLevel(indentLevel);
    LogStream::SetIndentSize(indentSize);
    LogStream::SetTrimFilenames(trim);
}

void TestCPPLogger::TestIndentLevel() {
    char stringName[64] = "Test Output.";

    LogStream::PrintAndCheckResult(stringName);
    assert(!strncmp(stringName, "Test Output.", 64));

    LogStream::SetIndentSize(0);
    LogStream::SetIndentLevel(1);
    LogStream::PrintAndCheckResult(stringName);
    assert(!strncmp(stringName, "Test Output.", 64));

    LogStream::SetIndentSize(1);
    LogStream::SetIndentLevel(1);
    LogStream::PrintAndCheckResult(stringName);
    assert(!strncmp(stringName, " Test Output.", 64));

    LogStream::SetIndentSize(2);
    LogStream::DecrementIndent();
    LogStream::PrintAndCheckResult(stringName);
    assert(!strncmp(stringName, " Test Output.", 64));

    LogStream::SetIndentSize(2);
    LogStream::IncrementIndent();
    LogStream::PrintAndCheckResult(stringName);
    assert(!strncmp(stringName, "   Test Output.", 64));

    LogStream::SetIndentSize(4);
    LogStream::IncrementIndent();
    LogStream::PrintAndCheckResult(stringName);
    assert(!strncmp(stringName, "           Test Output.", 64));

    LogStream::SetIndentSize(2);
    LogStream::SetIndentLevel(0);
}
#include <iostream>
void TestCPPLogger::TestTags() {
    const char *file;
    char testValue[64];
    char readBack[64];
    int line;

    char charArray[128] = "HA %f - %l HA";

    const char *constCharString = charArray;
    char *charString = charArray;
    const string constString(constCharString);
    string String(charString);

    // Pass in a const string variable.
    LogStream::ClearLogFile();
    Info(constString); file = __FILE__; line = __LINE__;
    snprintf(testValue, 128, "HA %s - %i HA", file, line);
    LogStream::CheckResult(readBack);
    assert(!strncmp(readBack, testValue, 128));

    // Pass in a string variable.
    LogStream::ClearLogFile();
    Info(String); file = __FILE__; line = __LINE__;
    snprintf(testValue, 128, "HA %s - %i HA", file, line);
    LogStream::CheckResult(readBack);
    assert(!strncmp(readBack, testValue, 128));

    // Pass in a char* variable.
    LogStream::ClearLogFile();
    Info(charString); file = __FILE__; line = __LINE__;
    snprintf(testValue, 128, "HA %s - %i HA", file, line);
    LogStream::CheckResult(readBack);
    assert(!strncmp(readBack, testValue, 128));

    // Pass in a const char* variable.
    LogStream::ClearLogFile();
    Info(constCharString); file = __FILE__; line = __LINE__;
    snprintf(testValue, 128, "HA %s - %i HA", file, line);
    LogStream::CheckResult(readBack);
    assert(!strncmp(readBack, testValue, 128));

    // Checks against a problem that arises with template specialization
    LogStream::ClearLogFile();
    Info("HA %f - %l HA"); file = __FILE__; line = __LINE__;
    snprintf(testValue, 128, "HA %s - %i HA", file, line);
    LogStream::CheckResult(readBack);
    assert(!strncmp(readBack, testValue, 128));

    // Checks against a problem that arises with template specialization
    LogStream::ClearLogFile();
    Info(charArray); file = __FILE__; line = __LINE__;
    snprintf(testValue, 128, "HA %s - %i HA", file, line);
    LogStream::CheckResult(readBack);
    assert(!strncmp(readBack, testValue, 128));

    // Checks against a problem that arises with template specialization
    LogStream::ClearLogFile();
    Info(string("HA %f - %l HA")); file = __FILE__; line = __LINE__;
    snprintf(testValue, 128, "HA %s - %i HA", file, line);
    LogStream::CheckResult(readBack);
    assert(!strncmp(readBack, testValue, 128));
}

void TestCPPLogger::TestPretext() {
    char stringName[64] = "Test Output.";
    LogStream::SetPretext("PRE:");
    LogStream::PrintAndCheckResult(stringName);
    assert(!strncmp(stringName, "PRE:Test Output.", 64));
    LogStream::SetPretext("");
}

void TestCPPLogger::TestLogTarget() {
    char stringName[64] = "Test Output.";
    LogStream::PrintAndCheckResult(stringName);
    assert(!strncmp(stringName, "Test Output.", 64));
}

void TestCPPLogger::TestLogLevel() {
    char testString[64] = "Test Output.";
    char current[64];
    
    for(int output = 0; output <= LogStream::NoOutput; output++) {
        for(int max = 0; max <= LogStream::NoOutput; max++) {
            LogStream::SetLogLevel((LogStream::LogType)max);
            strncpy(current, testString, 64);
            LogStream::PrintAndCheckResult(current, (LogStream::LogType)output);
            if (output < max) { assert(!strncmp(current, "", 64)); }
            else { assert(!strncmp(current, "Test Output.", 64)); }
        }
    }

    LogStream::SetLogLevel(LogStream::InfoMessage);
}

void TestCPPLogger::TestClearFile() {
    char one[64] = "A";
    char two[64] = "B";
    LogStream::PrintAndCheckResult(one);
    LogStream::PrintAndCheckResult(two);
    assert(!strncmp(two, "B", 64));
}
