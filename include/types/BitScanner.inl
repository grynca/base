#include "BitScanner.h"
#include "Index.h"
#include "base_cfg.h"
#include "functions/common.h"
#include "functions/meta.h"

#define BS_TPL template <typename WordType>
#define BS_TYPE BitScanner<WordType>

namespace grynca {

    BS_TPL
    inline BS_TYPE::BitScanner(const WordType* words, u32 words_cnt)
     : words_(words), words_cnt_(words_cnt)
    {
        reset();
    }

    BS_TPL
    inline void BS_TYPE::setData(const WordType* words, u32 words_cnt) {
        words_ = words;
        words_cnt_ = words_cnt;
    }

    BS_TPL
    inline bool BS_TYPE::operator++(int) {
#if USE_BITSCAN == 1
        pos_++;
        size_t wid = getWordOffset_(pos_);
        if (wid >= words_cnt_) {
            pos_ = InvalidId();
            return false;
        }
        WordType w = words_[wid];
        //Discard bits lower than _pos
        w &= (~(WordType)0) << getBitOffset_(pos_);

        //Check the first word
        if (w != (WordType)0) {
            pos_ = u32(ctz(w) + wid*BITS_IN_TYPE(WordType));
            return true;
        }
        //Check all the following words
        for(size_t i = wid+ 1; i < words_cnt_; i++){
            w = words_[i];
            if(w != 0){
                pos_ = u32(ctz(w) + i * BITS_IN_TYPE(WordType));
                return true;
            }
        }
        pos_ = InvalidId();
        return false;
#else
        u32 bits_cnt =  BITS_IN_TYPE(WordType) * words_cnt_;
        do {
            pos_++;
        }
        while(pos_ < bits_cnt && !testBit_(pos_));
        return pos_ < bits_cnt;
#endif /* USE_BITSCAN */
    }

    BS_TPL
    inline void BS_TYPE::setPos(u32 pos) {
        pos_ = pos;
    }

    BS_TPL
    inline u32 BS_TYPE::getPos()const {
        return pos_;
    }

    BS_TPL
    inline bool BS_TYPE::isValid()const {
        return pos_ != InvalidId();
    }

    BS_TPL
    inline void BS_TYPE::reset() {
        pos_ = InvalidId();
    }

    BS_TPL
    size_t BS_TYPE::getBitOffset_(size_t pos) {
        // static
        return pos % BITS_IN_TYPE(WordType);
    }

    BS_TPL
    size_t BS_TYPE::getWordOffset_(size_t pos) {
        // static
        return pos / BITS_IN_TYPE(WordType);
    }

    BS_TPL
    inline bool BS_TYPE::testBit_(size_t bit_id) {
        return words_[bit_id / BITS_IN_TYPE(WordType)] & (WordType(1) << (bit_id % BITS_IN_TYPE(WordType)));
    }

}

#undef BS_TPL
#undef BS_TYPE