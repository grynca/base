#include "Bits.h"

namespace grynca {

    inline Bits::Bits(u32 bits_cnt)
     : words_(wordsCount_(bits_cnt), 0), set_cnt_(0)
    {}

    inline bool Bits::operator[](size_t bit_id)const {
        return getWord_(bit_id) & getMask_(bit_id);
    }

    inline void Bits::resize(size_t bits_cnt) {
        words_.resize(wordsCount_(bits_cnt), 0);
    }

    inline void Bits::resizeOnes(size_t bits_cnt) {
        words_.resize(wordsCount_(bits_cnt), word_type(-1));
    }

    // returns if changed
    inline bool Bits::set(size_t bit_id) {
        ensureSize_(bit_id);
        return setFast(bit_id);
    }

    inline bool Bits::reset(size_t bit_id) {
        ensureSize_(bit_id);
        return resetFast(bit_id);
    }

    inline bool Bits::flip(size_t bit_id) {
        ensureSize_(bit_id);
        return flipFast(bit_id);
    }

    inline bool Bits::setFast(size_t bit_id) {
        word_type& w = accWord_(bit_id);
        word_type m = getMask_(bit_id);
        u32 prev_cnt = popcount(w);
        w |= m;
        u32 cnt = popcount(w);
        set_cnt_ +=  popcount(w) - prev_cnt;
        return prev_cnt != cnt;
    }

    inline bool Bits::resetFast(size_t bit_id) {
        word_type& w = accWord_(bit_id);
        word_type m = getMask_(bit_id);
        u32 prev_cnt = popcount(w);
        w &= ~m;
        u32 cnt = popcount(w);
        set_cnt_ +=  popcount(w) - prev_cnt;
        return prev_cnt != cnt;
    }

    inline bool Bits::flipFast(size_t bit_id) {
        word_type& w = accWord_(bit_id);
        word_type m = getMask_(bit_id);
        u32 prev_cnt = popcount(w);
        w ^= m;
        u32 cnt = popcount(w);
        set_cnt_ +=  popcount(w) - prev_cnt;
        return prev_cnt != cnt;
    }

    inline void Bits::clear() {
        setWords_(0);
        set_cnt_ = 0;
    }

    inline const Bits::word_type* Bits::getWords()const {
        return words_.begin();
    }

    inline Bits::word_type* Bits::accWords() {
        return words_.begin();
    }

    inline size_t Bits::getWordsCount()const {
        return words_.size();
    }

    inline size_t Bits::getBytesCount()const {
        return words_.size() * sizeof(word_type);
    }

    inline size_t Bits::getBitsCount() const {
        return words_.size() * word_bits;
    }

    inline size_t Bits::getOnesCount()const {
        return set_cnt_;
    }

    inline BitScanner<Bits::word_type> Bits::getIterator()const {
        return BitScanner<word_type>(words_.begin(), words_.size());
    }

    inline BitScanner<Bits::word_type> Bits::getIterator(u32 words_cnt)const {
        ASSERT(words_cnt <= words_.size());
        return BitScanner<word_type>(words_.begin(), words_cnt);
    }

    inline size_t Bits::wordsCount_(size_t bits_cnt) {
        // static
        return bits_cnt / word_bits + ((bits_cnt%word_bits)!=0);
    }

    inline size_t Bits::bitOffset_(size_t bit_id) {
        // static
        return bit_id % word_bits;
    }

    inline size_t Bits::wordOffset_(size_t bit_id) {
        //static
        return bit_id / word_bits;
    }

    inline Bits::word_type Bits::getMask_(size_t bit_id) {
        // static
        return word_type(1) << (bit_id % word_bits);
    }

    inline void Bits::ensureSize_(size_t bit_id) {
        size_t needed_words = wordOffset_(bit_id)+1;
        if (needed_words > words_.size()) {
            words_.resize(needed_words, 0);
        }
    }

    inline Bits::word_type Bits::getWord_(size_t bit_id)const {
        return words_[wordOffset_(bit_id)];
    }

    inline Bits::word_type& Bits::accWord_(size_t bit_id) {
        return words_[wordOffset_(bit_id)];
    }

    inline void Bits::setWords_(u8 val) {
        memset(words_.begin(), val, words_.size()*sizeof(word_type));
    }

    inline std::ostream& operator<<(std::ostream& os, const Bits& b) {
        std::string str;
        ssu::bitsToString(b.words_.begin(), b.words_.size(), str);
        os << str;
        return os;
    }
}