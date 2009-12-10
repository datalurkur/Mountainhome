/*
 *  CPPLogger.cpp
 *  SceneGraph
 *
 *  Created by Brent Wilson on 11/7/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "CPPLogger.h"
#include <iostream>
#include <fstream>

// TODO Why am I using regular assert instead of my ASSERT macros? I think it had to do with dependencies, but I don't remember.
#include <assert.h>

std::ofstream FileOut;

string LogStream::Logfile = "";
string LogStream::Pretext = "%f[%l]:%6 ";
LogStream::LogType LogStream::LogLevel = InfoMessage;
bool LogStream::AbortOnError = false;
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

bool LogStream::SetAbortOnError(bool abort) {
    bool oldVal = AbortOnError;
    AbortOnError = abort;
    return oldVal;
}

void LogStream::SetLogTarget(const string &filename) {
    Logfile = filename;
}

void LogStream::SetLogLevel(LogType level) {
    LogLevel = level;
}

void LogStream::ClearLogFile() {
    FileOut.open(Logfile.c_str(), std::ofstream::trunc);
    FileOut.close();
}

LogStream::LogStream(LogType type, bool end, const string &file, int line): _endline(end),
_type(type), _file(file), _line(""), _first(true) {
    char tmp[16];
    snprintf(tmp, 16, "%i", line);
    _line = tmp;

    _stream = &std::cout;
    if (Logfile.size()) {
        FileOut.open(Logfile.c_str(), std::ofstream::app);
        _stream = &FileOut;
    }
    
    if (_type >= LogLevel) {
        output(Pretext); _first = true;
        (*_stream) << string(IndentSize * IndentLevel, ' ');
    }
}

LogStream::~LogStream() {
    if (_type >= LogLevel) {
        if (_endline) {
            (*_stream) << std::endl;
        }
    }
    
    _stream->flush();
    FileOut.close();
    if (_type == ErrorMessage && AbortOnError) {
        assert(!"Aborting...");
    }
}

LogStream& LogStream::operator<<(char* rhs) {
    output(rhs);
    return *this;
}

LogStream& LogStream::operator<<(const char* rhs) {
    output(rhs);
    return *this;
}

LogStream& LogStream::operator<<(string &rhs) {
    output(rhs);
    return *this;
}

LogStream& LogStream::operator<<(const string &rhs) {
    output(rhs);
    return *this;
}

const string& LogStream::replaceTags(const string &original, string &temp) {
    temp = "";
    string tag;
    size_t current = 0, last = 0;

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
                temp += _file.substr(_file.rfind('/') + 1);
            } else {
                temp += _file;
            }
        } else if (tag == "l") { // Line Number
            temp += _line;
        } else if (tag == "t") { // Time
            AppendTimeString(temp);
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

void LogStream::AppendTimeString(string &timeless) {
    timeless += "03:07";
}

void LogStream::PrintAndCheckResult(char* string, LogType output) {
    // Clear the logfile and output the string.
    LogStream::ClearLogFile();
    LogStream::LogStream(output, true, __FILE__, __LINE__) << string;

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
