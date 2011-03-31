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

#define AreColorsEnabled() (!strncmp(getenv("XcodeColors") ?: "", "YES", 3))
//#define AreColorsEnabled() true

// Foreground
#define LCL_BLACK   "\033[0;30m"
#define LCL_RED     "\033[0;31m"
#define LCL_GREEN   "\033[0;32m"
#define LCL_YELLOW  "\033[0;33m"
#define LCL_BLUE    "\033[0;34m"
#define LCL_MAGENTA "\033[0;35m"
#define LCL_CYAN    "\033[0;36m"
#define LCL_WHITE   "\033[0;37m"

// Background
#define LBCL_BLACK      "\033[0;40m"
#define LBCL_RED        "\033[0;41m"
#define LBCL_GREEN      "\033[0;42m"
#define LBCL_YELLOW     "\033[0;43m"
#define LBCL_BLUE       "\033[0;44m"
#define LBCL_MAGENTA    "\033[0;45m"
#define LBCL_CYAN       "\033[0;46m"
#define LBCL_WHITE      "\033[0;47m"

// Reset colors
#define LCL_RESET       "\033[0m"

std::ofstream FileOut;

std::string LogStream::Logfile = "";
std::string LogStream::Pretext = "%f:%l |%8 ";
LogStream::LogType LogStream::LogLevel = InfoMessage;
LogChannel LogStream::ActiveLogChannels = LogStream::DefaultChannel;
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

void LogStream::EnableLogChannel(LogChannel channel) {
    ActiveLogChannels = (LogChannel)((int)ActiveLogChannels | (int)channel);
}

void LogStream::DisableLogChannel(LogChannel channel) {
    ActiveLogChannels = (LogChannel)((int)ActiveLogChannels & ~(int)channel);
}

void LogStream::EnableAllChannels() {
    ActiveLogChannels = (LogChannel)~0;
}

void LogStream::DisableAllChannels() {
    ActiveLogChannels = (LogChannel)0;
}

bool LogStream::IsChannelEnabled(LogChannel channel) {
    return (((int)ActiveLogChannels & (int)channel) != 0);
}

void LogStream::SetPretext(const std::string &text) {
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

void LogStream::SetLogTarget(const std::string &filename) {
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

const std::string& LogStream::ReplaceTags(const std::string &original, std::string &temp, const std::string &file, int line) {
    temp = "";
    std::string tag;
    size_t current = 0, last = 0;

    char tmp[16];
    snprintf(tmp, 16, "%i", line);
    std::string strLine(tmp);

    // Loop through the string as long as we can find another tag to process. Use last as
    // the starting point as it will always be pointing at the character directly after
    // the last tag.
    while ((current = original.find("%", last)) != std::string::npos) {
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
                temp = std::string(indended -  temp.size(), ' ') + temp;
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
    std::string file = "";

    if (Dest & Console) { console = &std::cout; }
    if (Dest & Disk)    { file    = Logfile;    }

    OutStream = new LogStream(console, file);
}

void LogStream::Flush() {
    if (OutStream) {
        // Reset here as it is called after each log call.
        if(AreColorsEnabled()) {
            (*OutStream) << LCL_RESET;
        }
        OutStream->flush();
    }
}

LogStream& LogStream::GetLogStream(LogType type, bool newline, const std::string &file, int line) {
    if (!NilStream) { NilStream = new LogStream(NULL, ""); }
    if (!OutStream) { CreateOutStream(); }
    if (BreakOnError && type == ErrorMessage) { ASSERT(0); }

    if (type < LogLevel) { return *NilStream; }

    if (newline) {
        std::string temp;
        (*OutStream) << "\n";
        PrintColorTagsForLevel(type);
        (*OutStream) << ReplaceTags(Pretext, temp, file, line);
        (*OutStream) << std::string(IndentSize * IndentLevel, ' ');
    } else {
        PrintColorTagsForLevel(type);
    }

    return *OutStream;
}

void LogStream::PrintColorTagsForLevel(LogType level) {
    if(!AreColorsEnabled()) { return; }
    switch(level) {
        case TraceMessage: break;
        case DebugMessage: break;
        case InfoMessage:  break;
        case WarningMessage: (*OutStream) << LCL_RED;  break;
        case ErrorMessage:   (*OutStream) << LBCL_RED; break;
    }
}

LogStream::LogStream(std::ostream *console, const std::string &filename):
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
