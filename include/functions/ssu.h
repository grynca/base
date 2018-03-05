//
// Created by lutza on 26.6.2015.
//

#ifndef SSU_H
#define SSU_H

#include "../3rdp/ustring.h"
#include "../types/containers/fast_vector.h"

// String&Stream utils

#define FLOAT_TO_STR(val, DigitsTotal, DigitsAfterDot) \
    [](f32 v) { \
        std::string rslt; \
        rslt.resize(DigitsTotal); \
        int cnt = snprintf(&rslt[0], DigitsTotal+1, "%"#DigitsTotal"."#DigitsAfterDot"f", v); \
        ASSERT(cnt >= 0 && cnt <= (DigitsTotal+1) ); \
        (void)cnt; \
        return rslt; \
    }(val)

#define DOUBLE_TO_STR(val, DigitsTotal, DigitsAfterDot) \
    [](f64 v) { \
        std::string rslt; \
        rslt.resize(DigitsTotal); \
        int cnt = snprintf(&rslt[0], DigitsTotal+1, "%"#DigitsTotal"."#DigitsAfterDot"lf", v); \
        ASSERT(cnt >= 0 && cnt <= (DigitsTotal+1) ); \
        (void)cnt; \
        return rslt; \
    }(val)

namespace grynca {
    class ssu {
    public:
        // when conversion is not possible, default val will be returned
        template <typename ST,typename T>
        static T fromString(const ST& str, const T& default_val);

        template <typename T>
        static std::string toStringA(const T& t);

        template <typename Container>
        static ustring arrToString(const Container& t);
        template <typename Container>
        static std::string arrToStringA(const Container& t);

        // e.g: prints 0.22 to 22%
        template <u32 Precision=3>
        static std::string printPercA(f32 p);

        // converts array of n T's to string of '1'&'0's, MSB is on left
        template <typename T>
        static void bitsToString(const T* vals, size_t n, std::string& str_out);
        // other way around
        template <typename T>
        static void stringToBits(const std::string& str, T* vals, size_t n);

        static ustring spaces(u32 cnt);
        static std::string spacesA(u32 cnt);

        template <typename T>
        static u32 countLines(const T& str);

        // also returns longest line
        template <typename T>
        static u32 countLinesLL(const T& str, u32& longest_line_out);

        // trim whitespace from start
        template <typename ST>
        static ST ltrim(const ST& s);
        // trim whitespace from end
        template <typename ST>
        static ST rtrim(const ST& s);
        // trim whitespace from both ends
        template <typename ST>
        static ST trim(const ST& s);

        template <typename ST>
        static ST removeSubstring(const ST& str, const ST& substr);

        template <typename ST>
        static void tokenize(const ST& str, fast_vector<ST>& tokens, const typename ST::value_type* delimiters);          // tokenize utf8 string

        // removes prefix upto first occurance of token
        // returns InvalidId() if token was not found
        // if remove_prefix then str is modified and prefix up to token is removed
        static u32 findSubstring(ustring& str, const ustring& substr, bool remove_prefix);

        template<typename ... Args>
        static ustring format( const ustring& format, Args&& ... args );
        template<typename ... Args>
        static std::string formatA( const std::string& format, Args&& ... args );

    // stream utils
        static bool getLineA(std::istream& source, ustring& dest);
        static bool getLineA(std::istream& source, std::string& dest);

        static u32 available(std::istream& stream);


    };
}


#include "ssu.inl"
#endif //SSU_H
