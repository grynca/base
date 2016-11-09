#include "string_utils.h"
#include <sstream>
#include <cctype>
#include <algorithm>

namespace grynca {
    template <typename T>
    inline T string_utils::fromString(const std::string& str, const T& default_val) {
        T val = default_val;
        std::istringstream ( str ) >> val;
        return val;
    }

    template <typename T>
    inline std::string string_utils::toString(const T& t) {
        std::ostringstream s;
        s << t;
        return s.str();
    }

    inline std::string string_utils::printPerc(f32 p) {
        if (fabs(p) < 0.00000001)
            return 0;
        return toString(((u32)(p*100000))/1000.0f);
    }

    inline std::string string_utils::ltrim(const std::string& s) {
        std::string ss = s;
        ss.erase(ss.begin(), std::find_if(ss.begin(), ss.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return ss;
    }

    inline std::string string_utils::rtrim(const std::string& s) {
        std::string ss = s;
        ss.erase(std::find_if(ss.rbegin(), ss.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), ss.end());
        return ss;
    }

    inline std::string string_utils::trim(const std::string& s) {
        return ltrim(rtrim(s));
    }

    inline std::string string_utils::removeSubstring(const std::string& str, const std::string& substr)  {
        std::string output = str;
        std::size_t pos = output.find(substr);
        if (pos != std::string::npos)
        {
            output.erase(pos, substr.size());
        }
        return output;
    }

    inline void string_utils::tokenize(const std::string& str, fast_vector<std::string>& tokens, const std::string& delimiters) {
        // Skip delimiters at beginning.
        std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
        // Find first "non-delimiter".
        std::string::size_type pos = str.find_first_of(delimiters, lastPos);

        while (std::string::npos != pos || std::string::npos != lastPos)
        {
            // Found a token, add it to the vector.
            tokens.push_back(str.substr(lastPos, pos - lastPos));
            // Skip delimiters. Note the "not_of"
            lastPos = str.find_first_not_of(delimiters, pos);
            // Find next "non-delimiter"
            pos = str.find_first_of(delimiters, lastPos);
        }
    }

    inline int string_utils::findSubstring(std::string& str, const std::string& substr, bool remove_prefix) {
        size_t pos = str.find(substr);
        if (pos!=std::string::npos) {
            if (remove_prefix) {
                pos += substr.length();
                str = str.substr(pos);
            }
            return pos;
        }
        else
            return -1;
    }
}
