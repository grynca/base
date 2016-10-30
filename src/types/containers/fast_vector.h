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
#include <string.h>
#include "../../functions/debug.h"

namespace grynca {

#define GROWING_FACTOR 1.5

    template<typename T>
    class fast_vector {
    public:
        // types:
        typedef T                                     value_type;
        typedef T&                                   reference;
        typedef const T&                             const_reference;
        typedef T*                                   pointer;
        typedef const T*                             const_pointer;
        typedef T*                                   iterator;
        typedef const T*                             const_iterator;
        typedef std::reverse_iterator<iterator>       reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef ptrdiff_t                             difference_type;
        typedef unsigned int                          size_type;

       // constructors
       fast_vector()
        : mCapacity(0), mSize(0), mData(NULL) { }

       fast_vector(size_t n)
        : mCapacity(n), mSize(n)
       {
          mData = (T *) malloc(sizeof(T) * n);
          for (size_t i = 0; i < n; ++i)
             new (&mData[i]) T();
       }

       fast_vector(size_t n, const T &val)
        : mCapacity(n), mSize(n)
       {
          mData = (T *) malloc(sizeof(T) * n);
          for (size_t i = 0; i < n; ++i)
             new (&mData[i]) T(val);
       }

       template<template<class> class InputIterator, class U>
       fast_vector(const InputIterator<U> first, const InputIterator<U> last)
       {
          size_t count = last - first;
          mCapacity = mSize = count;
          mData = (T *) malloc(sizeof(T) * count);
          for (size_t i = 0; i < count; ++i, ++first) {
             new (&mData[i]) T(*first);
          }
       }

       fast_vector(const fast_vector<T> &x)      // copy constructor
        : mCapacity(x.mCapacity), mSize(x.mSize)
       {
          mData = (T *) malloc(sizeof(T) * mCapacity);
          for (size_t i = 0; i < mSize; ++i) {
             new (&mData[i]) T(x.mData[i]);
          }
       }

        /// move constructor
        fast_vector(fast_vector<T> &&x)
         : mCapacity(x.mCapacity), mSize(x.mSize)
        {
           mData = (T *) malloc(sizeof(T) * mCapacity);
           for (size_t i = 0; i < mSize; ++i) {
              // call move constructors on items
              new (&mData[i]) T(std::move(x.mData[i]));
           }
        }

       // initializer list
       fast_vector(std::initializer_list<T> il) {
          size_t count = il.size();
          mCapacity = mSize = count;
          mData = (T *) malloc(sizeof(T) * count);
          for (size_t i=0; i<count; ++i) {
             new (&mData[i]) T(*(il.begin() + i));
          }
       }

       fast_vector<T> &operator=(const fast_vector<T> &x) {
          if (mCapacity < x.mSize) {
             _realloc_all(_get_new_capacity(capacity(), x.mSize));
          }
          size_t i = 0;
          for ( ; i< mSize; ++i) {
             mData[i] = x.mData[i];
          }
          for ( ; i<x.mSize; ++i) {
             new (&mData[i]) T(x.mData[i]);
          }

          mSize = x.mSize;
          return *this;
       }

       // move assignment
       fast_vector<T>& operator=(fast_vector<T>&& x) {
          this->~fast_vector();
          mCapacity = x.mCapacity;
          mSize = x.mSize;
          mData = x.mData;
          x.mData = NULL;
          x.mSize = 0;
          x.mCapacity = 0;
          return *this;
       }

        fast_vector<T>& operator=(std::initializer_list<T> il) {
           size_t count = il.size();
           if (mCapacity < count) {
              _realloc_all(_get_new_capacity(capacity(), count));
           }

           size_t i = 0;
           for ( ; i<mSize; ++i) {
              mData[i] = *(il.begin()+i);
           }
           for ( ; i<count; ++i) {
              new (&mData[i]) T(*(il.begin()+i));
           }

           return *this;
        }

       ~fast_vector() {
          for (size_t i=0; i<mSize; ++i) {
             mData[i].~T();
          }
          free(mData);
       }

       void push_back() {
          if (mSize == mCapacity) {
             _realloc_all(_get_new_capacity(capacity(), capacity() + 1));
          }
          new (&mData[mSize]) T();
          ++mSize;
       }

       void push_back(const T &el)      // uses copy constructor for T
       {
          if (mSize == mCapacity) {
             _realloc_all(_get_new_capacity(capacity(), capacity() + 1));
          }
          new (&mData[mSize]) T(el);
          ++mSize;
       }

        void push_back(T &&val) {
           if (mSize == mCapacity) {
              _realloc_all(_get_new_capacity(capacity(), capacity() + 1));
           }
           new (&mData[mSize]) T(std::move(val));
           ++mSize;
        }

        void pop_back() {
           --mSize;
           mData[mSize].~T(); // destroy last element
        }

       template<class... ConstructionArgs>
       void emplace_back(ConstructionArgs &&... args) {
          if (mSize == mCapacity) {
             _realloc_all(_get_new_capacity(capacity(), capacity() + 1));
          }
          new (&mData[mSize]) T(std::move(T(std::forward<ConstructionArgs>(args)...)));
          ++mSize;
       }

       T &operator[](size_t id) {
          ASSERT(id < mSize);
          return mData[id];
       }

       const T &operator[](size_t id) const {
          ASSERT(id < mSize);
          return mData[id];
       }

       size_t size() const {
          return mSize;
       }

       size_t sizeBytes() const {
          return mSize*sizeof(T);
       }

       iterator begin() {
          return mData;
       }

       const_iterator begin() const {
          return mData;
       }

       iterator end() {
          return mData+mSize;
       }

       const_iterator end() const {
          return mData+mSize;
       }

       reverse_iterator rbegin() {
          return reverse_iterator(mData+mSize);
       }

       const_reverse_iterator rbegin() const {
          return const_reverse_iterator(mData+mSize);
       }

       reverse_iterator rend() {
          return reverse_iterator(mData);
       }

       const_reverse_iterator rend() const {
          return const_reverse_iterator(mData);
       }

       void resize(size_t n, const T &val = T()) {
          if (n > mSize) {
             if (n > mCapacity) {
                _realloc_all(n);
             }
             for (size_t i=mSize; i<n; ++i) {
                new (&mData[i]) T(val);
             }
          }
          else {
             for (size_t i=n; i<mSize; ++i) {
                mData[i].~T();
             }
          }
          mSize = n;
       }

       size_t capacity() const {
          return mCapacity;
       }

       size_t capacityBytes() const {
          return mCapacity*sizeof(T);
       }

       bool empty() const {
          return mSize == 0;
       }

       void reserve(size_t n) {
          if (n > mCapacity) {
             _realloc_all(n);
          }
       }

       T &at(size_t id) {
          if (id >= mSize)
             throw std::out_of_range("fast_vector");
          return mData[id];
       }

       const T &at(size_t id) const {
          if (id >= mSize)
             throw std::out_of_range("fast_vector");
          return mData[id];
       }

       T &front() {
          return mData[0];
       }

       const T &front() const {
          return mData[0];
       }

       T &back() {
          return mData[mSize-1];
       }

       const T &back() const {
          return mData[mSize-1];
       }

       iterator insert(iterator position)  // requires copy constructor
       {
          return insert(position, T());
       }

       iterator insert(iterator position, const T &val) {
          ASSERT(position <= end() );
          ASSERT(position >= begin());
          if (mSize == mCapacity) {
             size_t pos_id = position-mData;
             _realloc_all(_get_new_capacity(capacity(), mSize + 1));
             position = &mData[pos_id];
          }

          memmove(position + 1, position, (uint8_t *)end() - (uint8_t *) position);
          new (position) T(val);
          ++mSize;
          return position;
       }

       template<class... ConstructionArgs>
       iterator emplace(iterator position, ConstructionArgs&&... args) {
          ASSERT(position <= end() );
          ASSERT(position >= begin());
          if (mSize == mCapacity) {
             size_t pos_id = position-mData;
             _realloc_all(_get_new_capacity(capacity(), mSize + 1));
             position = &mData[pos_id];
          }

          memmove(position + 1, position, (uint8_t *)end() - (uint8_t *) position);
          new (position) T(std::forward<ConstructionArgs>(args)...);
          ++mSize;
          return position;
       }

       iterator insert(iterator position, size_t n, const T &val) {
          if (mSize+n > mCapacity) {
             size_t pos_id = position-mData;
             _realloc_all(_get_new_capacity(capacity(), mSize + n));
             position = &mData[pos_id];
          }
          memmove(position + n, position, (uint8_t *)end() - (uint8_t *)position);
          mSize += n;
          for (iterator it = position; n--; ++it) {
             new (it) T(val);
          }

          return position;
       }

       template<typename InputIterator>
       iterator insert(iterator position, InputIterator first, InputIterator last) {
          size_t count = last - first;
          if (mSize + count > mCapacity) {
             size_t pos_id = position-mData;
             _realloc_all(_get_new_capacity(capacity(), mSize + count));
             position = &mData[pos_id];
          }
          memmove(position + count, position, (uint8_t *)end() - (uint8_t *)position);
          for (iterator it = position; first != last; ++it, ++first) {
             new (it) T(*first);
          }
          mSize += count;
          return position;
       }

       iterator erase(iterator position) {
          ASSERT(position < end() );
          ASSERT(position >= begin());

          position->~T();
          size_t tail_size = (uint8_t *)end() - (uint8_t *)position - sizeof(T);
          memmove(position, position+1, tail_size);
          --mSize;
          return position;
       }

       iterator erase(iterator first, iterator last) {
          ASSERT(first <= end());
          ASSERT(first >= begin());
          ASSERT(last <= end());
          ASSERT(last >= begin());
          ASSERT(last >= first);

          iterator tmp = first;
          while (tmp != last)  // call destructors for erased elems
          {
             tmp->~T();
             ++tmp;
          }
          size_t tail_size = (uint8_t *)end() - (uint8_t *)last;
          memmove(first, last, tail_size);
          mSize -= (last - first);
          return first;
       }

       // clear
       void clear() {
          size_type i;
          for (size_t i = 0; i<mSize; ++i)
             mData[i].~T();
          mSize = 0;
       }

       void swap(fast_vector<T> &x) {
          std::swap(mData, x.mData);
          std::swap(mSize, x.mSize);
          std::swap(mCapacity, x.mCapacity);
       }

       template<class InputIterator>
       void assign(InputIterator first, InputIterator last) {
          size_t count = last - first;
          if (mCapacity < count) {
             _realloc_all(_get_new_capacity(capacity(), count));
          }
          size_t i=0;
          for ( ; i < mSize; ++i, ++first) {
             mData[i] = *first;
          }
          for ( ; i < count; ++i, ++first) {
             new (&mData[i]) T(*first);
          }
          mSize = count;
       }

       void assign(size_t n, const T &val) {
          if (mCapacity < n) {
             _realloc_all(_get_new_capacity(capacity(), n));
          }
          size_t i=0;
          for ( ; i < mSize; ++i) {
             mData[i] = val;
          }
          for ( ; i <n; ++i) {
             new (&mData[i]) T(val);
          }
          mSize = n;
       }

    private:

       inline size_t _get_new_capacity(size_t curr_capacity, size_t needed_capacity) {
          return std::max((size_t) std::ceil(curr_capacity * GROWING_FACTOR), needed_capacity);
       }

       inline void _realloc_all(size_t new_capacity) {
          mCapacity = new_capacity;
          mData = (T *)realloc(mData, new_capacity*sizeof(T));
       }

       size_type mCapacity;
       size_type mSize;
       T *mData;
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
