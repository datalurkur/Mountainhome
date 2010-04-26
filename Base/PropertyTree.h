/*
 *  PersistentMap.h
 *  System
 *
 *  Created by loch on 2/7/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _PROPERTYTREE_H_
#define _PROPERTYTREE_H_
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "Vector.h"

namespace boost {
    namespace property_tree {
        /*! Defines a translator for the ptree that turns a set of numbers into a vector. */
        template <int Size>
        struct vector_translator {
            Vector<Size> get_value(const std::string &input) {
                // Break the string into tokens.
                std::vector<std::string> tokens;
                tokenize(input, " ", tokens);

                // Verify the number of tokens read is correct.
                if (tokens.size() != Size) {
                    THROW(InternalError, "Invalid vector size read from file. " << Size <<
                        " tokens expected, got " << tokens.size() << " from string: " << input);
                }

                // Fill in our new vector.
                Vector<Size> result;
                for (int i = 0; i < Size; i++) {
                    result[i] = strtod(tokens[i].c_str(), (char **)NULL);
                }

                return result;
            }

            typedef std::string internal_type;
            typedef Vector3     external_type;
        };

        // Define the translators to the various vector sizes.
        template <> struct translator_between<std::string, Vector2> { typedef vector_translator<2> type; };
        template <> struct translator_between<std::string, Vector3> { typedef vector_translator<3> type; };
        template <> struct translator_between<std::string, Vector4> { typedef vector_translator<4> type; };
    }
}

#endif
