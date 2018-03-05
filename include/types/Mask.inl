#include "Mask.h"
#include "Index.h"
#include "../functions/ssu.h"

#define MTYPE typename Mask<Size>

namespace grynca {

    template <size_t Size>
    inline constexpr Mask<Size>::Mask() {
        setWords_(0);
    }

    template <size_t Size>
    inline constexpr Mask<Size>::Mask(word_type v) {
        setWords_(0);

        if (Size < word_bits_) {
            // clear bits that dont fit to mask
            u32 shift = u32(word_bits_-Size);
            data_[0] = (v << shift) >> shift;
        }
        else {
            data_[0] = v;
        }
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
        if ((Size % word_bits_) != 0) {
            // zero out unused remainder of last word
            data_[words_cnt_-1] &= ONES(0, Size%word_bits_);
        }
    }

    template <size_t Size>
    inline void Mask<Size>::reverse() {
        // reverse order of words
        std::reverse(data_, data_+words_cnt_);
        // reverse bits
        for (u32 i=0; i<words_cnt_; ++i) {
            data_[i] = reverseBits_(data_[i]);
        }

        u32 shift = u32((words_cnt_*word_bits_)-Size);
        if (shift) {
            u32 overflow = 0;
            for (i32 i=i32(words_cnt_)-1; i>=0; --i) {
                word_type prev_overflow = overflow;
                overflow = data_[i]<<(word_bits_ - shift);
                data_[i] = (data_[i]>>shift) | prev_overflow;
            }
        }
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
    inline size_t Mask<Size>::findLastSetBit() {
        for (i32 i = i32(words_cnt_)-1; i >= 0; --i) {
            if (data_[i] == 0)
                continue;
            u32 last_bit_id = u32(word_bits_) - clz(data_[i]) - 1;
            return i*word_bits_ + last_bit_id;
        }
        NEVER_GET_HERE("dont call on empty mask");
        return InvalidId();
    }

    template <size_t Size>
    inline void Mask<Size>::insert(size_t bit_id, size_t cnt) {
        assert(bit_id < Size);
        assert(cnt < word_bits_);

        u32 w_id = u32(bit_id / word_bits_);
        u32 bit_in_w = u32(bit_id % word_bits_);

        // first word
        word_type mask = ONES(bit_in_w, word_bits_);
        word_type to_shift = data_[w_id]&mask;
        data_[w_id] = (data_[w_id]&(~mask)) | (to_shift<<cnt);
        word_type overflow = to_shift>>(word_bits_ - cnt);


        for (++w_id; w_id<words_cnt_; ++w_id) {
            word_type prev_overflow = overflow;
            overflow = data_[w_id]>>(word_bits_ - cnt);
            data_[w_id] = (data_[w_id]<<cnt) | prev_overflow;
        }
    }

    template <size_t Size>
    inline void Mask<Size>::remove(size_t bit_id, size_t cnt) {
        assert(bit_id < Size);
        assert(cnt < word_bits_);

        u32 w_id = u32(bit_id / word_bits_);
        u32 bit_in_w = u32(bit_id % word_bits_);

        word_type overflow = 0;
        for (i32 i=i32(words_cnt_)-1; i>i32(w_id); --i) {
            word_type prev_overflow = overflow;
            overflow = data_[i]<<(word_bits_ - cnt);
            data_[i] = (data_[i]>>cnt) | prev_overflow;
        }

        // last word
        word_type mask = word_type(ONES(0, bit_in_w));
        word_type lower_part = data_[w_id]&mask;
        data_[w_id] &= ~(word_type(ONES(0, bit_in_w+cnt)));
        data_[w_id] >>= cnt;
        data_[w_id] = (data_[w_id] | lower_part) | overflow;
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
        if ((Size % word_bits_) != 0) {
            u32 trail(word_bits_ - (Size % word_bits_));
            bs.erase(bs.begin(), bs.begin()+trail);
        }
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

    template <size_t S>
    inline bool constexpr operator==(const Mask<S>& m1, const Mask<S>& m2) {
        return std::equal(m1.data_, m1.data_+Mask<S>::words_cnt_, m2.data_);
    }

    template <size_t S>
    inline constexpr Mask<S> operator& (const Mask<S>& m1, const Mask<S>& m2) {
        Mask<S> rslt;
        for (u32 i=0; i<Mask<S>::words_cnt_; ++i) {
            rslt.data_[i] = m1.data_[i]&m2.data_[i];
        }
        return rslt;
    }

    template <size_t S1, size_t S2>
    inline constexpr Mask<std::max(S1, S2)> operator| (const Mask<S1>& m1, const Mask<S2>& m2) {
        constexpr size_t SMAX = std::max(S1, S2);
        constexpr size_t SMIN = std::min(S1, S2);

        Mask<SMAX> rslt = m1;
        if (S1 == SMAX) {
            for (u32 i=0; i<Mask<SMIN>::words_cnt_; ++i) {
                rslt.data_[i] |= m2.data_[i];
            }
        }
        else {
            for (u32 i=0; i<Mask<SMIN>::words_cnt_; ++i) {
                rslt.data_[i] |= m1.data_[i];
            }
        }
        return rslt;
    }

    template <size_t S>
    inline constexpr Mask<S> operator^(const Mask<S>& m1, const Mask<S>& m2) {
        Mask<S> rslt;
        for (u32 i=0; i<Mask<S>::words_cnt_; ++i) {
            rslt.data_[i] = m1.data_[i] ^ m2.data_[i];
        }
        return rslt;
    }

    template <size_t S>
    inline std::ostream& operator<<(std::ostream& os, const Mask<S>& m) {
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
        memset(data_, val, i32(words_cnt_*sizeof(word_type)));
    }

    template <size_t Size>
    inline typename Mask<Size>::word_type Mask<Size>::reverseBits_(word_type x) {
        static const unsigned char bit_reverse_table256[] = {
            0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
            0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
            0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
            0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
            0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
            0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
            0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
            0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
            0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
            0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
            0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
            0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
            0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
            0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
            0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
            0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
        };

        word_type rslt;
        u8* bytes_in = (u8*)&x;
        u8* bytes_out = (u8*)&rslt;
        u32 bytes_cnt = sizeof(word_type);
        for (u32 i=0; i<bytes_cnt; ++i) {
            bytes_out[bytes_cnt-1-i] = bit_reverse_table256[bytes_in[i]];
        }
        return rslt;
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