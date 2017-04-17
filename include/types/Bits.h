#ifndef BITS_H
#define BITS_H

#include "BitScanner.h"
#include "containers/fast_vector.h"
#include <cstring>

namespace grynca {

    // sorta like dynamic bitset

    class Bits {
    public:
        typedef u64 word_type;
        static constexpr u32 word_bits = BITS_IN_TYPE (word_type);

        Bits(u32 bits_cnt = 0);     // (rounded up to whole bytes)

        bool operator[](size_t bit_id)const;

        // (rounded up to whole bytes)
        // added bits are initialized to 0's
        void resize(size_t bits_cnt);
        // added bits are initialized to 1's
        void resizeOnes(size_t bits_cnt);

        // returns if changed, also lazyly resizes if neccessary
        bool set(size_t bit_id);
        bool reset(size_t bit_id);
        bool flip(size_t bit_id);

        // returns if changed, does not check size
        bool setFast(size_t bit_id);
        bool resetFast(size_t bit_id);
        bool flipFast(size_t bit_id);

        void clear();

        // words access
        const word_type* getWords()const;

        word_type* accWords();

        size_t getWordsCount()const;
        size_t getBytesCount()const;
        size_t getBitsCount()const;
        size_t getOnesCount()const;

        BitScanner<word_type> getIterator()const;

        BitScanner<word_type> getIterator(u32 words_cnt)const;
    private:
        friend std::ostream& operator<<(std::ostream& os, const Bits& b);

        static size_t wordsCount_(size_t bits_cnt);
        static size_t bitOffset_(size_t bit_id);
        static size_t wordOffset_(size_t bit_id);
        static word_type getMask_(size_t bit_id);

        void ensureSize_(size_t bit_id);
        word_type getWord_(size_t bit_id)const;
        word_type& accWord_(size_t bit_id);
        void setWords_(u8 val);

        fast_vector<word_type> words_;
        u32 set_cnt_;
    };
}

#define LOOP_SET_BITS(BITS, IT) \
    auto IT = (BITS).getIterator(); \
    while (IT++)

#include "Bits.inl"
#endif //BITS_H
