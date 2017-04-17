#include "Mask.h"
#include "Index.h"

#define MTYPE typename Mask<Size>
#define WORD_T typename Mask<S>::word_type

namespace grynca {

    template <size_t Size>
    inline constexpr Mask<Size>::Mask() {
        setWords_(0);
    }

    template <size_t Size>
    inline constexpr Mask<Size>::Mask(word_type v) {
        setWords_(0);
        data_[0] = v;
    }

    template <size_t Size>
    inline constexpr Mask<Size>::Mask(const std::string& buf)	{
        ssu::stringToBits(buf, data_, words_cnt_);
    }

    template <size_t Size>
    inline constexpr Mask<Size> Mask<Size>::bit(size_t bit_id) {
        // static
        Mask m;
        m.set(bit_id);
        return m;
    }


    template <size_t Size>
    template <size_t Size2>
    inline Mask<Size>::Mask(const Mask<Size2>& m2) {
        ASSERT(m2.size() <= size());
        setWords_(0);
        memcpy(data_, m2.data_, sizeof(m2.data_));
    }

    template <size_t Size>
    inline void Mask<Size>::flip (size_t n) {
        accWord_(n) ^= getMask_(n);
    }

    template <size_t Size>
    inline void Mask<Size>::reset() {
        setWords_(0);
    }

    template <size_t Size>
    inline void Mask<Size>::clear() {
        setWords_(0);
    }

    template <size_t Size>
    inline void	Mask<Size>::set() {
        setWords_(u8(-1));
    }

    template <size_t Size>
    inline Mask<Size> Mask<Size>::operator~()const {
        Mask<Size> rv (*this);
        rv.flip();
        return rv;
    }

    template <size_t Size>
    inline constexpr size_t Mask<Size>::size() {
        // size
        return Size;
    }

    template <size_t Size>
    inline size_t Mask<Size>::capacity()const {
        return bits_cnt_;
    }

    template <size_t Size>
    inline bool Mask<Size>::test(size_t n)const	{
        return getWord_(n) & getMask_(n);
    }

    template <size_t Size>
    inline bool Mask<Size>::operator[](size_t n)const {
        return test(n);
    }

    template <size_t Size>
    inline void Mask<Size>::flip() {
        for (u32 i = 0; i < words_cnt_; ++i) {
            data_[i] = ~data_[i];
        }
    }

    template <size_t Size>
    inline void Mask<Size>::set(size_t n) {
        accWord_(n) |=  getMask_(n);
    }

    template <size_t Size>
    inline void	Mask<Size>::set(size_t n, bool val) {
        word_type& w(accWord_(n));
        const word_type m(getMask_(n));
        const word_type bOn (w | m), bOff (w & ~m);
        w = val ? bOn : bOff;
    }

    template <size_t Size>
    inline void	Mask<Size>::reset(size_t n) {
        accWord_(n) &= ~getMask_(n);
    }

    // Returns a string with bits MSB "001101001..." LSB.
    template <size_t Size>
    inline std::string Mask<Size>::to_string()const {
        std::string bs;
        ssu::bitsToString(data_, words_cnt_, bs);
        return bs;
    }

    template <size_t Size>
    inline const MTYPE::word_type* Mask<Size>::getWords()const {
        return data_;
    }

    template <size_t Size>
    inline MTYPE::word_type* Mask<Size>::accWords() {
        return data_;
    }

    template <size_t Size>
    inline constexpr size_t Mask<Size>::getWordsCount() {
        // static
        return words_cnt_;
    }

    /// Returns the value in bits \p first through \p last.
    template <size_t Size>
    inline MTYPE::word_type Mask<Size>::getWordMask(size_t first, size_t last)const {
        assert ( (last-first) <= word_bits_ && "Bit ranges must be smaller than word");
        assert (first / word_bits_ == last / word_bits_ && "Bit ranges can not cross word boundary");
        return (getWord_(first) >> (first % word_bits_)) & LSB_MASK(word_type, (last-first));
    }

    template <size_t Size>
    inline void	Mask<Size>::setWordMask(size_t first, size_t last, word_type v) {
        assert ((last-first) <= word_bits_ && "Bit ranges must be smaller than word");
        assert (first / word_bits_ == last / word_bits_ && "Bit ranges can not cross word boundary");
        assert ((v & LSB_MASK(word_type, (last-first))) == v && "The value is too large to fit in the given bit range");
        accWord_(first) |= v << (first % word_bits_);
    }

    template <size_t Size>
    inline MaskBitIterator<Size> Mask<Size>::getIterator()const {
        return MaskBitIterator<Size>(*this);
    }

    template <size_t Size>
    inline bool Mask<Size>::any()const	{
        for (u32 i=0; i<words_cnt_; ++i) {
            if (data_[i]!=0)
                return true;
        }
        return false;
    }

    template <size_t Size>
    inline bool Mask<Size>::none()const {
        return !any();
    }

    template <size_t Size>
    inline size_t Mask<Size>::count()const {
        size_t sum = 0;
        for (u32 i=0; i<words_cnt_; ++i) {
            sum += popcount(data_[i]);
        }
        return sum;
    }

    template <size_t Size>
    inline bool constexpr Mask<Size>::operator== (ref_self v)const {
        return std::equal(data_, data_+words_cnt_, v.data_);
    }

    template <size_t Size>
    inline constexpr Mask<Size> Mask<Size>::operator& (ref_self v)const {
        Mask<Size> rslt;
        for (u32 i=0; i<words_cnt_; ++i) {
            rslt.data_[i] = data_[i]&v.data_[i];
        }
        return rslt;
    }

    template <size_t Size>
    inline constexpr Mask<Size> Mask<Size>::operator| (ref_self v)const {
        Mask<Size> rslt;
        for (u32 i=0; i<words_cnt_; ++i) {
            rslt.data_[i] = data_[i] | v.data_[i];
        }
        return rslt;
    }

    template <size_t Size>
    inline constexpr Mask<Size> Mask<Size>::operator^(ref_self v)const {
        Mask<Size> rslt;
        for (u32 i=0; i<words_cnt_; ++i) {
            rslt.data_[i] = data_[i] ^ v.data_[i];
        }
        return rslt;
    }

    template <size_t Size>
    inline constexpr MTYPE::ref_self Mask<Size>::operator&= (ref_self v) {
        for (u32 i=0; i<words_cnt_; ++i) {
            data_[i] &= v.data_[i];
        }
        return *this;
    }

    template <size_t Size>
    inline constexpr MTYPE::ref_self Mask<Size>::operator|= (ref_self v) {
        for (u32 i=0; i<words_cnt_; ++i) {
            data_[i] |= v.data_[i];
        }
        return *this;
    }

    template <size_t Size>
    inline constexpr MTYPE::ref_self Mask<Size>::operator^= (ref_self v) {
        for (u32 i=0; i<words_cnt_; ++i) {
            data_[i] ^= v.data_[i];
        }
        return *this;
    }

    template <size_t Size>
    inline std::ostream& operator<<(std::ostream& os, const Mask<Size>& m) {
        // static
        os << m.to_string();
        return os;
    }

    template <size_t Size>
    inline MTYPE::word_type& Mask<Size>::accWord_(size_t bit_id) {
        assert (bit_id < Size);
        return data_ [bit_id / word_bits_];
    }

    template <size_t Size>
    inline MTYPE::word_type Mask<Size>::getWord_(size_t bit_id)const {
        assert (bit_id < Size);
        return data_ [bit_id / word_bits_];
    }

    template <size_t Size>
    inline MTYPE::word_type Mask<Size>::getMask_(size_t bit_id) const {
        assert (bit_id < Size);
        return word_type(1) << (bit_id % word_bits_);
    }

    template <size_t Size>
    inline void Mask<Size>::setWords_(u8 val) {
        memset(data_, val, words_cnt_*sizeof(word_type));
    }

    template <size_t S>
    inline MaskBitIterator<S>::MaskBitIterator(const Mask<S>& m)
     : BitScanner<typename Mask<S>::word_type>(m.getWords(), m.getWordsCount()),
       mask_(m)
    {
    }

    template <size_t S>
    inline const Mask<S>& MaskBitIterator<S>::getMask()const {
        return mask_;
    }
}

#undef MTYPE
#undef USE_BITSCAN
#undef WORD_T