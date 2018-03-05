#include "ssu.h"
#include "3rdp/ustring.h"
#include "types/Index.h"
#include <sstream>
#include <cmath>
#include <algorithm>

namespace grynca {
    template <typename ST, typename T>
    inline T ssu::fromString(const ST& str, const T& default_val) {
        T val = default_val;
        std::istringstream ( str.c_str() ) >> val;
        return val;
    }

    template <typename T>
    inline std::string ssu::toStringA(const T& t) {
        std::ostringstream s;
        s << t;
        return s.str();
    }

    template <typename Container>
    inline ustring ssu::arrToString(const Container& t) {
        return arrToStringA(t);
    };

    template <typename Container>
    inline std::string ssu::arrToStringA(const Container& t) {
        std::ostringstream s;
        for (u32 i=0; i<t.size(); ++i) {
            s << t[i] << " ";
        }
        return s.str();
    };

    template <u32 Precision>
    inline std::string ssu::printPercA(f32 p) {
        if (fabsf(p) < 0.00000001f)
            return "0";
        return toStringA(((u32)(p*static_pow(10, Precision+2)))/f32(static_pow(10, Precision)));
    }

    template <typename T>
    inline void ssu::bitsToString(const T* vals, size_t n, std::string& str_out) {
        // static
        // (untested)
        str_out.resize(n*BITS_IN_TYPE(T), '0');
        u32 char_id = u32(str_out.length()-1);
        for (size_t i = 0; i < n; ++i) {
            for (T bmask = 1; bmask; bmask <<= 1) {
                str_out[char_id] = (vals[i]&bmask)?'1':'0';
                --char_id;
            }
        }
    }

    template <typename T>
    inline void ssu::stringToBits(const std::string& str, T* vals, size_t n) {
        // static
        // (untested)
        memset(vals, 0, n*sizeof(T));
        u32 char_id = u32(str.length()-1);
        for (size_t i = 0; i < n; ++i) {
            for (T bmask = 1; bmask; bmask <<= 1) {
                if (str[char_id] == '0') {
                    vals[i] &= ~bmask;
                }
                else {
                    vals[i] |= bmask;
                }
                if (char_id == 0)
                    break;
                --char_id;
            }
        }
    }
    inline ustring ssu::spaces(u32 cnt) {
        return ustring(cnt, ' ');
    }

    inline std::string ssu::spacesA(u32 cnt) {
        return std::string(cnt, ' ');
    }

    template <typename T>
    inline u32 ssu::countLines(const T& str) {
        u32 cnt = 1;
        for (u32 i=0; i<str.size(); ++i) {
            if (str[i] == '\n')
                ++cnt;
        }
        return cnt;
    }

    template <typename T>
    inline u32 ssu::countLinesLL(const T& str, u32& longest_line_out) {
        u32 cnt = 1;
        longest_line_out = 0;
        u32 linesize = 0;
        for (u32 i=0; i<str.size(); ++i) {
            if (str[i] == '\n') {
                if (linesize > longest_line_out)
                    longest_line_out = linesize;
                linesize = 0;
                ++cnt;
            }
            else {
                ++linesize;
            }
        }
        // last line
        if (linesize > longest_line_out)
            longest_line_out = linesize;
        return cnt;
    }

    template <typename ST>
    inline ST ssu::ltrim(const ST& s) {
        ST ss = s;
        ss.erase(ss.begin(), std::find_if(ss.begin(), ss.end(), [](char c) { return !isspace(c); }));
        return ss;
    }

    template <typename ST>
    inline ST ssu::rtrim(const ST& s) {
        ST ss = s;
        auto it1 =  std::find_if(ss.rbegin(), ss.rend(), [](char c) { return !isspace(c); });
        ss.erase(it1.base(), ss.end());
        return ss;
    }

    template <typename ST>
    inline ST ssu::trim(const ST& s) {
        return ltrim(rtrim(s));
    }

    template <typename ST>
    inline ST ssu::removeSubstring(const ST& str, const ST& substr)  {
        ST output = str;
        size_t pos = output.find(substr);
        if (pos != ST::npos)
        {
            output.erase(pos, substr.size());
        }
        return output;
    }

    template <typename ST>
    inline void ssu::tokenize(const ST& str, fast_vector<ST>& tokens, const typename ST::value_type* delimiters) {
        // Skip delimiters at beginning.
        typename ST::size_type lastPos = str.find_first_not_of(delimiters, 0);
        // Find first "non-delimiter".
        typename ST::size_type pos = str.find_first_of(delimiters, lastPos);

        while (ST::npos != pos || ST::npos != lastPos)
        {
            // Found a token, add it to the vector.
            tokens.push_back(str.substr(lastPos, pos - lastPos));
            // Skip delimiters. Note the "not_of"
            lastPos = str.find_first_not_of(delimiters, pos);
            // Find next "non-delimiter"
            pos = str.find_first_of(delimiters, lastPos);
        }
    }

    inline u32 ssu::findSubstring(ustring& str, const ustring& substr, bool remove_prefix) {
        size_t pos = str.find(substr);
        if (pos!=ustring::npos) {
            if (remove_prefix) {
                pos += substr.length();
                str = str.substr(pos);
            }
            return u32(pos);
        }
        else
            return InvalidId();
    }

    template<typename ... Args>
    inline ustring ssu::format( const ustring& format, Args&& ... args ) {
        // static
        size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
        std::unique_ptr<char[]> buf( new char[ size ] );
        snprintf( buf.get(), size, format.c_str(), args ... );
        return ustring( buf.get(), size - 1 ); // We don't want the '\0' inside
    }

    template<typename ... Args>
    inline std::string ssu::formatA( const std::string& format, Args&& ... args ) {
        // static
        size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
        std::unique_ptr<char[]> buf( new char[ size ] );
        snprintf( buf.get(), size, format.c_str(), args ... );
        return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
    }

    inline bool ssu::getLineA(std::istream& source, ustring& dest) {
    //static
        std::string ansi_line;
        bool good = std::getline(source, ansi_line).good();
        dest = ustring::ascii_constant(ansi_line);
        return good;
    }

    inline bool ssu::getLineA(std::istream& source, std::string& dest) {
        return std::getline(source, dest).good();
    }

    inline u32 ssu::available(std::istream& stream) {
    // static
        std::streampos pos = stream.tellg();
        stream.seekg( 0, std::ios::end );
        std::streamsize len = stream.tellg() - pos;
        stream.seekg( pos );
        return u32(len);
    }

}
