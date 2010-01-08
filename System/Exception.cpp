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

Exception::Exception(const std::string& desc, const char* src, const char* type,
    const char* file, unsigned int line): _line(line), _typeName(type),
    _description(desc), _source(src), _file(file) {
    Error(this->getFullDescription());
}

const std::string& Exception::getFullDescription(void) const {
    if (_fullDesc.empty()) {
        std::ostringstream desc;
        desc <<  "EXCEPTION(" << _typeName << "): " << _description << " in " << _source;

        if (_line > 0) {
            desc << " at " << _file << " (line " << _line << ")";
        }

        _fullDesc = desc.str();
    }

    return _fullDesc;
}
