#ifndef BITSCANNER_H
#define BITSCANNER_H

#include "functions/defs.h"

namespace grynca {

    // loops only through 1's
    template <typename WordType>
    class BitScanner {
    public:
        BitScanner(const WordType* words, u32 words_cnt);

        void setData(const WordType* words, u32 words_cnt);

        bool operator++(int);

        void setPos(u32 pos);
        u32 getPos()const;
        bool isValid()const;
        void reset();

        const WordType* getWords() { return words_; }
        u32 getWordsCount()const { return words_cnt_; }
    protected:
        static size_t getBitOffset_(size_t pos);
        static size_t getWordOffset_(size_t pos);
        bool testBit_(size_t bit_id);

        const WordType* words_;
        u32 words_cnt_;
        u32 pos_;
    };

}

#include "BitScanner.inl"
#endif //BITSCANNER_H
