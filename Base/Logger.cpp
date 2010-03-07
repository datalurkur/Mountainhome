/*
 *  Logger.cpp
 *  Base
 *
 *  Created by loch on 12/27/09.
 *  Copyright 2009 Brent Wilson. All rights reserved.
 *
 */

#include "Assertion.h"
#include "Logger.h"
#include <iostream>
#include <fstream>

// TODO Why am I using regular assert instead of my ASSERT macros? I think it had to do with dependencies, but I don't remember.
#include <assert.h>

std::ofstream FileOut;

string LogStream::Logfile = "";
string LogStream::Pretext = "%f:%l |%7 ";
LogStream::LogType LogStream::LogLevel = InfoMessage;
LogStream::LogDestination LogStream::Dest = All;
LogStream *LogStream::OutStream = NULL;
LogStream *LogStream::NilStream = NULL;
bool LogStream::BreakOnError = false;
int LogStream::IndentLevel = 0;
int LogStream::IndentSize = 3;
bool LogStream::TrimFilenames = true;

void LogStream::SetIndentLevel(int level) {
    IndentLevel = level;
}

void LogStream::IncrementIndent(int number) {
    IndentLevel += number;
}

void LogStream::DecrementIndent(int number) {
    IndentLevel -= number;
}

void LogStream::SetTrimFilenames(bool val) {
    TrimFilenames = val;
}

int LogStream::SetIndentSize(int size) {
    int oldVal = IndentSize;
    IndentSize = size;
    return oldVal;
}

void LogStream::SetPretext(const string &text) {
    Pretext = text;
}

bool LogStream::SetBreakOnError(bool newVal) {
    bool oldVal = BreakOnError;
    BreakOnError = newVal;
    return oldVal;
}

void LogStream::SetLogDestination(LogDestination dest) {
    DeleteOutStream();
    Dest = dest;
}

void LogStream::SetLogTarget(const string &filename) {
    DeleteOutStream();
    Logfile = filename;
}

void LogStream::SetLogLevel(LogType level) {
    DeleteOutStream();
    LogLevel = level;
}

void LogStream::ClearLogFile() {
    DeleteOutStream();
    FileOut.open(Logfile.c_str(), std::ofstream::trunc);
    FileOut.close();
}

void LogStream::PrintAndCheckResult(char* string, LogType output) {
    // Clear the logfile and output the string.
    LogStream::ClearLogFile();
    LogStream::GetLogStream(output, true, __FILE__, __LINE__) << string;
    LogStream::CheckResult(string);
}

void LogStream::CheckResult(char* string) {
    char tmp[2];

    // Read the string back in from the file.
    std::ifstream checkResult(LogStream::Logfile.c_str(), std::fstream::in);
    checkResult.getline(string, 64);
    checkResult.getline(tmp, 2);
    assert(checkResult.eof());
    checkResult.close();
}

const string& LogStream::ReplaceTags(const string &original, string &temp, const string &file, int line) {
    temp = "";
    string tag;
    size_t current = 0, last = 0;

    char tmp[16];
    snprintf(tmp, 16, "%i", line);
    string strLine(tmp);

    // Loop through the string as long as we can find another tag to process. Use last as
    // the starting point as it will always be pointing at the character directly after
    // the last tag.
    while ((current = original.find("%", last)) != string::npos) {
        // Add everything from the last tag up to this one to the temp string.
        temp += original.substr(last, current - last);

        // If the last character in the string is a % we need to bail early or we'll get
        // out of bounds issues when we try and look up the tag letter.
        if (current + 1 >= original.length()) {
            break;
        }

        // Just grab and compare the letter after the % symbol. This is nice because in
        // the case where no match is found we can just append what we get here. This
        // means that '%%' will output just '%', much like printf.
        tag = original.substr(current + 1, 1);

        if (tag == "f") { // File Name
            if (TrimFilenames) {
                temp += file.substr(file.rfind('/') + 1);
            } else {
                temp += file;
            }
        } else if (tag == "l") { // Line Number
            temp += strLine;
        } else if (tag == "t") { // Time
            //\todo Implement time.
        } else if (tag.c_str()[0] <= '9' || tag.c_str()[0] >= '1') {
            int indended = ((tag.c_str()[0] - '0') * 4);
            if (temp.size() < indended) {
                temp = string(indended -  temp.size(), ' ') + temp;
            }
        } else {                 // Whatever else
            temp += tag;
        }

        // Set last to the caracter directly after the tag.
        last = current + 2;
    }

    temp += original.substr(last);
    return temp.length() ? temp : original;
}

void LogStream::DeleteOutStream() {
    delete OutStream;
    OutStream = NULL;
}

void LogStream::CreateOutStream() {
    std::ostream *console = NULL;
    string file = "";

    if (Dest & Console) { console = &std::cout; }
    if (Dest & Disk)    { file    = Logfile;    }

    OutStream = new LogStream(console, file);
}

void LogStream::Flush() {
    if (OutStream) { OutStream->flush(); }
}

LogStream& LogStream::GetLogStream(LogType type, bool newline, const string &file, int line) {
    if (!NilStream) { NilStream = new LogStream(NULL, ""); }
    if (!OutStream) { CreateOutStream(); }
    if (BreakOnError && type == ErrorMessage) { ASSERT(0); }

    if (type < LogLevel) { return *NilStream; }

    if (newline) {
        string temp;
        (*OutStream) << "\n";
        (*OutStream) << ReplaceTags(Pretext, temp, file, line);
        (*OutStream) << string(IndentSize * IndentLevel, ' ');
    }

    return *OutStream;
}

LogStream::LogStream(std::ostream *console, const string &filename):
_console(console), _file(NULL) {
    if (filename.size() > 0) {
        _file = new std::fstream();
        _file->open(filename.c_str(), std::ofstream::app);
        if (_file->fail()) {
            delete _file;
            _file = NULL;
        }
    }
}

LogStream::~LogStream() {
    _console->flush();

    if (_file) {
        _file->close();
        delete _file;
        _file = NULL;
    }
}

void LogStream::flush() {
    if (_file   ) { _file->flush();    }
    if (_console) { _console->flush(); }
}
