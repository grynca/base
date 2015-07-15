#ifndef FAST_VECTOR_H
#define FAST_VECTOR_H

#include <stdint.h>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <cassert>
#include <cmath>
#include <type_traits>
#include <memory>

namespace grynca {
    template<typename T>
    class fast_vec_iterator : public std::iterator<std::random_access_iterator_tag, T> {
    private:
       template<typename U> friend
       class fast_vector;

       template<typename U> friend
       class fast_vec_iterator;

       T *p;

       fast_vec_iterator(T *x) : p(x) { }    // available only to friends
    public:
       fast_vec_iterator() { }

       /// unary operators
       fast_vec_iterator &operator++() {
          ++p;
          return *this;
       }

       fast_vec_iterator operator++(int) { return fast_vec_iterator(p++); }         // postfix
       fast_vec_iterator &operator--() {
          --p;
          return *this;
       }

       fast_vec_iterator operator--(int) { return fast_vec_iterator(p--); }         // postfix
       fast_vec_iterator operator+(size_t n) const { return fast_vec_iterator(p + n); }

       fast_vec_iterator operator-(size_t n) const { return fast_vec_iterator(p - n); }

       fast_vec_iterator &operator+=(size_t n) {
          p += n;
          return *this;
       }

       fast_vec_iterator &operator-=(size_t n) {
          p -= n;
          return *this;
       }

       T &operator[](int n) { return *(operator+(n)); }

       template<typename U, typename V>
       friend int operator-(const fast_vec_iterator<U> &lhs, const fast_vec_iterator<V> &rhs);

       template<typename U, typename V>
       friend bool operator==(const fast_vec_iterator<U> &lhs, const fast_vec_iterator<V> &rhs);

       template<typename U, typename V>
       friend bool operator!=(const fast_vec_iterator<U> &lhs, const fast_vec_iterator<V> &rhs);

       template<typename U, typename V>
       friend bool operator<(const fast_vec_iterator<U> &lhs, const fast_vec_iterator<V> &rhs);

       template<typename U, typename V>
       friend bool operator>(const fast_vec_iterator<U> &lhs, const fast_vec_iterator<V> &rhs);

       template<typename U, typename V>
       friend bool operator<=(const fast_vec_iterator<U> &lhs, const fast_vec_iterator<V> &rhs);

       template<typename U, typename V>
       friend bool operator>=(const fast_vec_iterator<U> &lhs, const fast_vec_iterator<V> &rhs);

       T &operator*() const { return *p; }

       T *operator->() const { return p; }

       operator fast_vec_iterator<const T>() const { return fast_vec_iterator<const T>(p); }
    };

    template<typename U, typename V>
    inline int operator-(const fast_vec_iterator<U> &lhs, const fast_vec_iterator<V> &rhs) {
       return (int) (lhs.p - rhs.p);
    }

    /// Comparison operators
    template<typename U, typename V>
    inline bool operator==(const fast_vec_iterator<U> &lhs, const fast_vec_iterator<V> &rhs) {
       return lhs.p == rhs.p;
    }

    template<typename U, typename V>
    inline bool operator!=(const fast_vec_iterator<U> &lhs, const fast_vec_iterator<V> &rhs) {
       return lhs.p != rhs.p;
    }

    template<typename U, typename V>
    inline bool operator<(const fast_vec_iterator<U> &lhs, const fast_vec_iterator<V> &rhs) {
       return lhs.p < rhs.p;
    }

    template<typename U, typename V>
    inline bool operator>(const fast_vec_iterator<U> &lhs, const fast_vec_iterator<V> &rhs) {
       return lhs.p > rhs.p;
    }

    template<typename U, typename V>
    inline bool operator<=(const fast_vec_iterator<U> &lhs, const fast_vec_iterator<V> &rhs) {
       return !(lhs.p > rhs.p);
    }

    template<typename U, typename V>
    inline bool operator>=(const fast_vec_iterator<U> &lhs, const fast_vec_iterator<V> &rhs) {
       return !(lhs.p < rhs.p);
    }


    //#ifdef _DEBUG
    //#  define DEFAULT_INITIAL_ALLOC (size_t)2
    //#else // !_DEBUG
    //#  define DEFAULT_INITIAL_ALLOC (size_t)8
    //#endif // !_DEBUG
#define GROWING_FACTOR 1.5

    template<typename T>
    class fast_vector {
    public:
       typedef T value_type;
       typedef size_t size_type;
       typedef T &reference;
       typedef const T &const_reference;

       typedef fast_vec_iterator<T> iterator;
       typedef fast_vec_iterator<const T> const_iterator;

       typedef std::reverse_iterator<iterator> reverse_iterator;
       typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

       // constructors
       fast_vector()
               : mStart(NULL), mEnd(NULL), mAllocEnd(NULL) { }

       fast_vector(size_t n) {
          mStart = (T *) malloc(sizeof(T) * n);
          mAllocEnd = mStart + n;
          mEnd = mStart;
          for (size_t i = 0; i < n; ++i)
             push_back();
       }

       fast_vector(size_t n, const T &val) {
          mStart = (T *) malloc(sizeof(T) * n);
          mAllocEnd = mStart + n;
          mEnd = mStart;
          for (size_t i = 0; i < n; ++i)
             push_back(val);
       }

       template<template<class> class InputIterator, class U>
       fast_vector(const InputIterator<U> first, const InputIterator<U> last)
               : mStart(NULL), mEnd(NULL), mAllocEnd(NULL) {
          assign(first, last);
       }

       fast_vector(const fast_vector<T> &x)      // copy constructor
               : mStart(NULL), mEnd(NULL), mAllocEnd(NULL) {
          reserve(x.size());
          for (const_iterator it = x.begin(); it != x.end(); ++it)
             push_back(*it);
       }

       // initializer list
       fast_vector(std::initializer_list<T> il)
               : mStart(NULL), mEnd(NULL), mAllocEnd(NULL) {
          insert(begin(), il.begin(), il.end());
       }

       /// move constructor
       fast_vector(fast_vector &&rhs) {
          mStart = rhs.mStart;
          mEnd = rhs.mEnd;
          mAllocEnd = rhs.mAllocEnd;
          rhs.mStart = NULL;
          rhs.mEnd = NULL;
          rhs.mAllocEnd = NULL;
       }

       fast_vector &operator=(const fast_vector &x) {
          assign(x.begin(), x.end());
          return *this;
       }

       ~fast_vector() {
          T *tmp = mStart;
          while (tmp != mEnd) {
             tmp->~T();
             ++tmp;
          }
          free(mStart);
       }

       void push_back() {
          if (mEnd == mAllocEnd) {
             size_t curr_capacity = capacity();
             size_t new_alloc_bytes = _get_new_capacity(curr_capacity, curr_capacity + 1) * sizeof(T);
             _realloc_all(new_alloc_bytes);
          }
          std::allocator<T>::construct(mEnd);
          ++mEnd;
       }

       void push_back(const T &el)      // uses copy constructor for T
       {
          if (mEnd == mAllocEnd) {
             size_t curr_capacity = capacity();
             size_t new_alloc_bytes = _get_new_capacity(curr_capacity, curr_capacity + 1) * sizeof(T);
             _realloc_all(new_alloc_bytes);
          }
          new(mEnd) T(el);
          ++mEnd;
       }

       template<class... ConstructionArgs>
       void emplace_back(ConstructionArgs &&... args) {
          if (mEnd == mAllocEnd) {
             size_t curr_capacity = capacity();
             size_t new_alloc_bytes = _get_new_capacity(curr_capacity, curr_capacity + 1) * sizeof(T);
             _realloc_all(new_alloc_bytes);
          }
          new(mEnd) T(std::forward<ConstructionArgs>(args)...);
          ++mEnd;
       }

       void pop_back() {
          --mEnd;
          mEnd->~T(); // destroy last element
       }

       T &operator[](size_t id) {
          assert(mStart + id < mEnd);
          return *(mStart + id);
       }

       const T &operator[](size_t id) const {
          assert(mStart + id < mEnd);
          return *(mStart + id);
       }

       size_t size() const {
          return mEnd - mStart;
       }

       size_t sizeBytes() const {
          return (uint8_t *) mEnd - (uint8_t *) mStart;
       }

       iterator begin() {
          return mStart;
       }

       const_iterator begin() const {
          return mStart;
       }

       iterator end() {
          return mEnd;
       }

       const_iterator end() const {
          return mEnd;
       }

       reverse_iterator rbegin() {
          return reverse_iterator(mEnd - 1);
       }

       const_reverse_iterator rbegin() const {
          return const_reverse_iterator(mEnd - 1);
       }

       reverse_iterator rend() {
          return reverse_iterator(mStart - 1);
       }

       const_reverse_iterator rend() const {
          return const_reverse_iterator(mStart - 1);
       }

       void resize(size_t n, const T &val = T()) {
          size_t curr_size_bytes = sizeBytes();
          size_t new_size_bytes = n * sizeof(T);
          int size_diff = (int) (new_size_bytes - curr_size_bytes);
          if (size_diff > 0) {
             if (new_size_bytes > capacityBytes()) {
                _realloc_all(new_size_bytes);
             }
             T *ptr = (T *) ((uint8_t *) mEnd + size_diff);
             while (mEnd != ptr) {
                new(mEnd) T(val);
                ++mEnd;
             }
          }
          else {
             T *ptr = (T *) ((uint8_t *) mEnd + size_diff);
             while (ptr != mEnd) {
                ptr->~T();
                ++ptr;
             }
             mEnd = (T *) ((uint8_t *) mStart + new_size_bytes);
          }
       }

       size_t capacity() const {
          return mAllocEnd - mStart;
       }

       size_t capacityBytes() const {
          return (uint8_t *) mAllocEnd - (uint8_t *) mStart;
       }

       bool empty() const {
          return mStart == mEnd;
       }

       void reserve(size_t n) {
          size_t desired_alloc = n * sizeof(T);
          size_t current_alloc = capacityBytes();
          if (desired_alloc > current_alloc) {
             _realloc_all(desired_alloc);
          }
       }

       T &at(size_t id) {
          T *elptr = mStart + id;
          if (elptr >= mEnd)
             throw std::out_of_range("fast_vector");
          return *elptr;
       }

       const T &at(size_t id) const {
          T *elptr = mStart + id;
          if (elptr >= mEnd)
             throw std::out_of_range("fast_vector");
          return *elptr;
       }

       T &front() {
          return *begin();
       }

       const T &front() const {
          return *begin();
       }

       T &back() {
          return *(end() - 1);
       }

       const T &back() const {
          return *(end() - 1);
       }

       iterator insert(iterator position)  // requires copy constructor
       {
          return insert(position, T());
       }

       iterator insert(iterator position, const T &val) {
          assert(position <= iterator(mEnd));
          assert(position >= iterator(mStart));
          if (mEnd < mAllocEnd)
             // no need for realloc yet
          {
             memmove(position.p + 1, position.p, (uint8_t *) mEnd - (uint8_t *) position.p);
             std::allocator<T>::construct(position.p, val);
             ++mEnd;
             return position;
          }
          else
             // no space here -> find new home for data;]
          {
             size_t curr_capacity = capacity();
             size_t new_capacity_bytes = _get_new_capacity(curr_capacity, curr_capacity + 1) * sizeof(T);

             size_t head_size_bytes = (uint8_t *) position.p - (uint8_t *) mStart;
             T *oldStart = _realloc_split_by_gap(new_capacity_bytes, head_size_bytes, sizeof(T));
             // create element in gap
             uint8_t *gap_slot = (uint8_t *) mStart + head_size_bytes;
             std::allocator<T>::construct((T *) gap_slot, val);
             free(oldStart);
             return (T *) (gap_slot);
          }
       }

       void insert(iterator position, size_t n, const T &val) {
          fill_insert(position, n, val);
       }

       template<typename InputIterator>
       void insert(iterator position, InputIterator first, InputIterator last) {
          insert_range_or_fill(position, first, last);
       }

       iterator erase(iterator position) {
          assert(position < iterator(mEnd));
          assert(position >= iterator(mStart));
          if (position.p == (mEnd - 1)) {
             pop_back();
          }
          else {
             (position.p)->~T();
             size_t tail_size = (uint8_t *) mEnd - (uint8_t *) position.p - sizeof(T);
             memmove(position.p, position.p + 1, tail_size);
             --mEnd;
          }
          return position;
       }

       iterator erase(iterator first, iterator last) {
          assert(first <= iterator(mEnd));
          assert(first >= iterator(mStart));
          assert(last <= iterator(mEnd));
          assert(last >= iterator(mStart));
          assert(last >= first);

          iterator tmp = first;
          while (tmp != last)  // call destructors for erased elems
          {
             (tmp.p)->~T();
             ++tmp;
          }
          size_t tail_size = (uint8_t *) mEnd - (uint8_t *) last.p;
          memmove(first.p, last.p, tail_size);
          mEnd -= (last - first);
          return first;
       }

       // clear
       void clear() {
          T *loop_ptr = mStart;
          while (loop_ptr != mEnd) {
             loop_ptr->~T();
             ++loop_ptr;
          }
          mEnd = mStart;
       }

       void swap(fast_vector<T> &x) {
          std::swap(mStart, x.mStart);
          std::swap(mEnd, x.mEnd);
          std::swap(mAllocEnd, x.mAllocEnd);
       }

       void assign(const_iterator first, const_iterator last) {
          if (first == last) {
             clear();
             return;
          }

          if ((mStart <= first.p && first.p <= mEnd) && (mStart <= last.p && last.p <= mEnd))
             self_assign(first, last);
          else
             normal_assign(first, last);
       }

       template<class InputIterator>
       void assign(InputIterator first, InputIterator last) {
          normal_assign(first, last);
       }

       void assign(size_t n, const T &val) {
          clear();
          reserve(n);
          for (size_t i = 0; i < n; ++i)
             push_back(val);
       }


    private:
       template<class InputIterator>
       void self_assign(InputIterator first, InputIterator last) {
          fast_vector<T> v(first, last);
          clear();
          std::swap(*this, v);
       }

       template<class InputIterator>
       void normal_assign(InputIterator first, InputIterator last) {
          clear();
          size_t range = std::distance(first, last);
          reserve(range);
          while (first != last) {
             push_back(*first);
             ++first;
          }
       }

       void fill_insert(iterator position, size_t n, const T &val) {
          if ((mEnd + n) <= mAllocEnd)
             // no need for realloc
          {
             memmove(position.p + n, position.p, (uint8_t *) mEnd - (uint8_t *) position.p);
             for (unsigned int i = 0; i < n; ++i) {
                std::allocator<T>().construct(position.p, val);
                ++position;
             }
             mEnd += n;
          }
          else
             // must realloc
          {
             size_t curr_capacity = capacity();
             size_t new_capacity_bytes = _get_new_capacity(curr_capacity, curr_capacity + n) * sizeof(T);
             size_t gap_size_bytes = n * sizeof(T);
             size_t head_size_bytes = (uint8_t *) position.p - (uint8_t *) mStart;
             T *oldStart = _realloc_split_by_gap(new_capacity_bytes, head_size_bytes, gap_size_bytes);
             // create elements in gap
             uint8_t *gap_slot = (uint8_t *) mStart + head_size_bytes;
             for (unsigned int i = 0; i < n; ++i) {
                std::allocator<T>().construct((T *) gap_slot, val);
                gap_slot += sizeof(T);
             }
             free(oldStart);
          }
       }

       template<typename Integer>
       typename std::enable_if<std::is_integral<Integer>::value>::type insert_range_or_fill(iterator position,
                                                                                            Integer n, Integer val) {
          fill_insert(position, n, val);
       }

       // For integral types only:
       template<typename InputIterator>
       typename std::enable_if<!std::is_integral<InputIterator>::value>::type insert_range_or_fill(iterator position,
                                                                                                   InputIterator first,
                                                                                                   InputIterator last) {
          insert_range(position, first, last);
       }

       template<typename InputIterator>
       void insert_range(iterator position, InputIterator first, InputIterator last) {
          size_t range = std::distance(first, last);

          if ((mEnd + range) <= mAllocEnd)
             // no need for realloc
          {
             memmove(position.p + range, position.p, (uint8_t *) mEnd - (uint8_t *) position.p);
             while (first != last) {
                std::allocator<T>().construct(position.p, *first);
                ++position;
                ++first;
             }
             mEnd += range;
          }
          else
             // must realloc
          {
             size_t curr_capacity = capacity();
             size_t new_capacity_bytes = _get_new_capacity(curr_capacity, curr_capacity + range) * sizeof(T);
             size_t gap_size_bytes = range * sizeof(T);
             size_t head_size_bytes = (uint8_t *) position.p - (uint8_t *) mStart;
             T *oldStart = _realloc_split_by_gap(new_capacity_bytes, head_size_bytes, gap_size_bytes);
             // create elements in gap
             uint8_t *gap_slot = (uint8_t *) mStart + head_size_bytes;
             while (first != last) {
                std::allocator<T>().construct((T *) gap_slot, *first);
                gap_slot += sizeof(T);
                ++first;
             }
             free(oldStart);
          }
       }

       size_t _get_new_capacity(size_t curr_capacity, size_t needed_capacity) {
          return std::max((size_t) std::ceil(curr_capacity * GROWING_FACTOR), needed_capacity);
       }

       void _realloc_all(size_t new_capacity_bytes) {
          size_t curr_size = sizeBytes();
          mStart = (T *) realloc(mStart, new_capacity_bytes);
          mEnd = (T *) ((uint8_t *) mStart + curr_size);
          mAllocEnd = (T *) ((uint8_t *) mStart + new_capacity_bytes);
       }

       // reallocs with gap inside
       T *_realloc_split_by_gap(size_t new_capacity_bytes, size_t head_size_bytes, size_t gap_size_bytes) {
          uint8_t *new_start = (uint8_t *) malloc(new_capacity_bytes);
          size_t tail_size_bytes = sizeBytes() - head_size_bytes;
          memcpy(new_start, mStart, head_size_bytes);
          memcpy(new_start + gap_size_bytes + head_size_bytes, (uint8_t *) mStart + head_size_bytes, tail_size_bytes);
          T *oldStart = mStart;

          mStart = (T *) new_start;
          mEnd = (T *) (new_start + head_size_bytes + tail_size_bytes + gap_size_bytes);
          mAllocEnd = (T *) (new_start + new_capacity_bytes);
          return oldStart;
       }

       T *mStart;
       T *mEnd;
       T *mAllocEnd;
    };



    // compare ops

    /// Less than
    template<typename T>
    inline bool operator<(const fast_vector<T> &lhs, const fast_vector<T> &rhs) {
       return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template<typename T, template<class, class> class Other, typename Allocator>
    inline bool operator<(const fast_vector<T> &lhs, const Other<T, Allocator> &rhs) {
       return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template<typename T, template<class, class> class Other, typename Allocator>
    inline bool operator<(const Other<T, Allocator> &lhs, const fast_vector<T> &rhs) {
       return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }


    /// Greater than
    template<typename T>
    inline bool operator>(const fast_vector<T> &lhs, const fast_vector<T> &rhs) {
       return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
    }

    template<typename T, template<class, class> class Other, typename Allocator>
    inline bool operator>(const fast_vector<T> &lhs, const Other<T, Allocator> &rhs) {
       return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
    }

    template<typename T, template<class, class> class Other, typename Allocator>
    inline bool operator>(const Other<T, Allocator> &lhs, const fast_vector<T> &rhs) {
       return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
    }

    /// Less then or equal
    template<typename T>
    inline bool operator<=(const fast_vector<T> &lhs, const fast_vector<T> &rhs) {
       return !std::operator>(lhs, rhs);
    }

    template<typename T, template<class, class> class Other, typename Allocator>
    inline bool operator<=(const fast_vector<T> &lhs, const Other<T, Allocator> &rhs) {
       return !std::operator>(lhs, rhs);
    }

    template<typename T, template<class, class> class Other, typename Allocator>
    inline bool operator<=(const Other<T, Allocator> &lhs, const fast_vector<T> &rhs) {
       return !std::operator>(lhs, rhs);
    }

    /// Greater then or equal
    template<typename T>
    inline bool operator>=(const fast_vector<T> &lhs, const fast_vector<T> &rhs) {
       return !std::operator<(lhs, rhs);
    }

    template<typename T, template<class, class> class Other, typename Allocator>
    inline bool operator>=(const fast_vector<T> &lhs, const Other<T, Allocator> &rhs) {
       return !std::operator<(lhs, rhs);
    }

    template<typename T, template<class, class> class Other, typename Allocator>
    inline bool operator>=(const Other<T, Allocator> &lhs, const fast_vector<T> &rhs) {
       return !std::operator<(lhs, rhs);
    }

    /// Equal
    template<typename T>
    inline bool operator==(const fast_vector<T> &lhs, const fast_vector<T> &rhs) {
       if (lhs.size() != rhs.size())
          return false;
       return std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template<typename T, template<class, class> class Other, typename Allocator>
    inline bool operator==(const fast_vector<T> &lhs, const Other<T, Allocator> &rhs) {
       if (lhs.size() != rhs.size())
          return false;
       return std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template<typename T, template<class, class> class Other, typename Allocator>
    inline bool operator==(const Other<T, Allocator> &lhs, const fast_vector<T> &rhs) {
       return std::operator==(rhs, lhs);
    }

    /// Not equal
    template<typename T>
    inline bool operator!=(const fast_vector<T> &lhs, const fast_vector<T> &rhs) {
       return !operator==(lhs, rhs);
    }

    template<typename T, template<class, class> class Other, typename Allocator>
    inline bool operator!=(const fast_vector<T> &lhs, const Other<T, Allocator> &rhs) {
       return !std::operator==(lhs, rhs);
    }

    template<typename T, template<class, class> class Other, typename Allocator>
    inline bool operator!=(const Other<T, Allocator> &lhs, const fast_vector<T> &rhs) {
       return !std::operator==(lhs, rhs);
    }


    template<class T>
    void swap(fast_vector<T> &a, fast_vector<T> &b) {
       a.swap(b);
    }
}

#endif // FAST_VECTOR_H
