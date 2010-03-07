/*
 *  Exception.cpp
 *  Base
 *
 *  Created by loch on 12/24/09.
 *  Copyright 2009 Brent Wilson. All rights reserved.
 *
 */

#include "Exception.h"
#include "Logger.h"

#include <sstream>

Exception::Exception(const std::string& desc, const char* type, const char* src,
    const char* file, unsigned int line): _line(line), _typeName(type),
    _description(desc), _source(src), _file(file) {

    Error("");
    Error("Exception(" << _typeName << ") in " << _source);
    LogStream::IncrementIndent();
    Error(_file << ":" << _line);
    Error(_description);
    LogStream::DecrementIndent();
    Error("");
}

const std::string& Exception::getFullDescription(void) const {
    if (_fullDesc.empty()) {
        std::ostringstream desc;
        desc <<  "Exception(" << _typeName << ") in " << _source << std::endl;
        desc << _file << ":" << _line << std::endl;
        desc << _description << std::endl;
        _fullDesc = desc.str();
    }

    return _fullDesc;
}
