#ifndef MASK_H
#define MASK_H

#include "../functions/debug.h"
#include "BitScanner.h"
#include <cassert>
#include <cstring>

namespace grynca {

    // fw
    template <size_t Size> class MaskBitIterator;

    template <size_t Size>
    class Mask {
    public:
        typedef u64	word_type;
        typedef const Mask<Size>&	ref_self;
    public:
        constexpr Mask();
        constexpr Mask(word_type v);      // set first word
        constexpr Mask(const std::string& buf);
        static constexpr Mask<Size> bit(size_t bit_id);        // named constructor for single-bit mask

        template <size_t Size2>
        Mask(const Mask<Size2>& m2);
        void flip(size_t n);
        void reset();
        void clear();
        void set();
        void reverse();
        Mask operator~()const;
        constexpr static size_t size();
        size_t capacity()const;
        bool test(size_t n)const;
        bool operator[](size_t n)const;

        // error when called on empty Mask, check with .any() before
        // returns id of last set bit
        size_t findLastSetBit();
        // cnt rightmost bits from last word are lost,
        //  inserted bits are zeroed
        void insert(size_t bit_id, size_t cnt);
        // bits are shifted to fill the gap,
        //  last word is padded with zeros on right
        void remove(size_t bit_id, size_t cnt);

        /// Flips all the bits in the set.
        void flip();
        void set(size_t n);
        /// Sets or clears bit \p n.
        void set(size_t n, bool val);

        /// Clears the bit \p n.
        void reset (size_t n);
        // Returns a string with bits MSB "001101001..." LSB.
        std::string to_string()const;

        // words access
        const word_type* getWords()const;
        word_type* accWords();
        static constexpr size_t getWordsCount();
        word_type getWordMask(size_t first, size_t last)const;      /// Returns the value in bits \p first through \p last.
        void setWordMask(size_t first, size_t last, word_type v);   // Sets the value of the bitrange \p first through \p last to the equivalent number of bits from \p v.

        MaskBitIterator<Size> getIterator()const;

        bool any()const;
        bool none()const;

        size_t count()const;
        constexpr ref_self operator&=(ref_self v);
        constexpr ref_self operator|=(ref_self v);
        constexpr ref_self operator^=(ref_self v);
    private:
        template <size_t S>
        friend class Mask;

        template <size_t S>
        friend constexpr bool operator==(const Mask<S>& m1, const Mask<S>& m2);
        template <size_t S>
        friend constexpr Mask<S> operator&(const Mask<S>& m1, const Mask<S>& m2);
        template <size_t S1, size_t S2>
        friend constexpr Mask<std::max(S1, S2)> operator|(const Mask<S1>& m1, const Mask<S2>& m2);
        template <size_t S>
        friend constexpr Mask<S> operator^(const Mask<S>& m1, const Mask<S>& m2);
        template <size_t S>
        friend std::ostream& operator<<(std::ostream& os, const Mask<S>& m);

        word_type& accWord_(size_t bit_id);
        word_type getWord_(size_t bit_id)const;
        word_type getMask_(size_t n)const;
        void setWords_(u8 val);
        word_type reverseBits_(word_type x);

        static constexpr size_t word_bits_ = BITS_IN_TYPE (word_type);
        static constexpr size_t words_cnt_ = Size / word_bits_ + ((Size % word_bits_) != 0);
        static constexpr size_t bits_cnt_ = words_cnt_ * word_bits_;

        word_type data_ [words_cnt_];
    };

    template <size_t S>
    class MaskBitIterator : public BitScanner<typename Mask<S>::word_type> {
    public:
        MaskBitIterator(const Mask<S>& m);
        const Mask<S>& getMask()const;
    private:
        const Mask<S>& mask_;
    };

    template <size_t Size>
    static std::ostream& operator<<(std::ostream& os, const Mask<Size>& m);
}

#include "Mask.inl"
#endif //MASK_H
