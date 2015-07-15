//
// Created by lutza on 26.6.2015.
//

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include "../types/containers/fast_vector.h"

namespace grynca {
    class string_utils {
    public:
        // when conversion is not possible, default val will be returned
        template <typename T>
        static T fromString(const std::string& str, const T& default_val);

        template <typename T>
        static std::string toString(const T& t);

        // trim whitespace from start
        static std::string ltrim(const std::string& s);
        // trim whitespace from end
        static std::string rtrim(const std::string& s);
        // trim whitespace from both ends
        static std::string trim(const std::string& s);

        static std::string removeSubstring(const std::string& str, const std::string& substr);

        static void tokenize(const std::string& str, fast_vector<std::string>& tokens, const std::string& delimiters);

        // removes prefix upto first occurance of token
        // returns -1 if token was not found
        // if remove_prefix then str is modified and prefix up to token is removed
        static int findSubstring(std::string& str, const std::string& substr, bool remove_prefix);
    };
}


#include "string_utils.inl"
#endif //STRING_UTILS_H
