//
//  Util.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Util_h
#define Project_Unknown_Util_h

#include "Platform.h"
#include "SharedPtr.h"
#include "Preprocessor.h"

namespace ukn {
    
    template<typename T>
    class Array {
    public:
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef T& reference;
        typedef const T& const_reference;
        
        typedef Array<T> self_type;
        
        enum { InvalidIndex = -1 };
        
        Array();
        Array(size_t capcity, size_t grow);
        Array(size_t size, size_t grow, const T& value);
        Array(const Array<T>& rhs);
        Array(const T* arr, size_t size);
        ~Array();
        
        void operator=(const Array<T>& rhs);
        T& operator[](size_t index) const;
        
        bool operator==(const Array<T>& rhs) const;
        bool operator!=(const Array<T>& rhs) const;
        
        void append(const T& elm);
        void appendArray(const Array<T>& rhs);
        void append(const T* arr, size_t size);
        
        void reserve(size_t num);
        size_t size() const;
        size_t capacity() const;
        
        T& front() const;
        T& back() const;
        
        bool empty() const;
        
        void erase(size_t index);
        iterator erase(iterator iter);
        
        void insert(size_t index, const T& elm);
        void insert(const T& elm);
        
        void push_back(const T& elm);
        void push_front(const T& elm);
        void pop_back();
        void pop_front();
        
        iterator begin();
        iterator end();
        
        const_iterator begin() const;
        const_iterator end() const;
        
        void clear();
        void reset();
        
        iterator find(const T& elm) const;
        size_t findIndex(const T& elm) const;
        
        void fill(size_t start, size_t num, const T& elm);
        void realloc(size_t size, size_t grow);
        
        /* Sort and Search */
        void insertSorted(const T& elm);
        bool isSorted() const;
        void sort() const;
        size_t binarySearchIndex(const T& elm);
        
        void map(T* mappedPtr, size_t size);
        void set(T* ptr, size_t size);
        
    private:
        void destroyAll();
        void destroy(T* elm);
        void copy(const Array<T>& src);
        void grow();
        void growTo(size_t capacity);
        void move(size_t index1, size_t index2);
        
        size_t mGrow;
        size_t mCapacity;
        size_t mSize;
        T* mElements;
        
        typedef SharedPtr<T, SharedPtrRefCounter> Ptr;
        Ptr mHolder;
        
        bool mMapped;
    };
    
    template<typename T>
    Array<T>::Array():
    mGrow(0),
    mCapacity(0),
    mSize(0),
    mElements(0),
    mMapped(false) {
        
    }
    
    template<typename T>
    Array<T>::Array(size_t capacity, size_t grow):
    mGrow(grow),
    mCapacity(capacity),
    mSize(0),
    mMapped(false) {
        if(capacity != 0) {
            this->mElements = ukn_malloc_t(T, capacity);
            this->mHolder = this->mElements;
            this->mSize = this->mCapacity;
        } else 
            this->mElements = 0;
    }
    
    template<typename T>
    Array<T>::Array(size_t size, size_t grow, const T& value):
    mGrow(grow),
    mCapacity(size),
    mSize(size),
    mMapped(false) {
        if(size > 0) {
            this->mElements = (T*)sora_malloc(sizeof(T) * this->mCapacity);
            for(int32 i = 0; i < this->mCapacity; ++i) {
                this->mElements[i] = value;
            }
            this->mHolder = this->mElements;
        } else {
            this->mElements = 0;
        }
    }
    
    template<typename T>
    Array<T>::Array(const T* arr, size_t size):
    mGrow(0),
    mCapacity(0),
    mSize(0),
    mMapped(false) {
        ukn_assert(size > 0);
        
        reserve(size);
        memcpy(this->mElements, arr, sizeof(T) * size);
        this->mSize = size;
    }
    
    template<typename T>
    void Array<T>::copy(const Array<T>& src) {
        ukn_assert(this->mElements == 0);
        
        this->mGrow = src.mGrow;
        this->mCapacity = src.mCapacity;
        this->mSize = src.mSize;
        if(this->mCapacity > 0) {
            this->mElements = (T*)sora_malloc(sizeof(T) * this->mCapacity);
            for(size_t i=0; i<this->mSize; ++i) {
                this->mElements[i] = src.mElements[i];
            }
            this->mHolder = this->mElements;
        }
    }
    
    template<typename T>
    void Array<T>::clear() {
        this->mGrow = 0;
        this->mCapacity = 0;
        this->mSize = 0;
        if(this->mElements) {
            // reset auto ptr
            this->mHolder.reset();
            
            this->mElements = 0;
        }
    }
    
    template<typename T>
    void Array<T>::map(T* mappedPtr, size_t size) {
        clear();
        
        this->mCapacity = this->mSize = size;
        this->mElements = mappedPtr;
        this->mMapped = true;
    }
    
    template<typename T>
    void Array<T>::set(T* ptr, size_t size) {
        clear();
        
        this->mElements = ptr;
        this->mMapped = false;
        this->mHolder = ptr;
        this->mCapacity = this->mSize = size;
    }
    
    template<typename T>
    void Array<T>::destroy(T* ele) {
        ele->~T();
    }
    
    template<typename T>
    Array<T>::Array(const Array<T>& rhs):
    mGrow(0),
    mCapacity(0),
    mSize(0),
    mElements(0),
    mMapped(rhs.mMapped) {
        *this = rhs;
    }
    
    template<typename T>
    Array<T>::~Array() {
        
    }
    
    template<typename T>
    void Array<T>::realloc(size_t capacity, size_t grow) {
        this->destroyAll();
        this->mGrow = grow;
        this->mCapacity = capacity;
        this->mMapped= false;
        this->mSize = 0;
        if(this->mCapacity > 0) {
            this->mElements = ukn_malloc_t(T, this->mCapacity);
            this->mHolder = this->mElements;
        } else {
            this->mElements = 0;
        }
    }
    
    template<typename T>
    void Array<T>::operator=(const Array<T>& rhs) {
        if(this != &rhs) {
            this->mGrow = rhs.mGrow;
            this->mCapacity = rhs.mCapacity;
            this->mSize = rhs.mSize;
            this->mMapped = rhs.mMapped;
            if(this->mCapacity > 0) {
                this->mElements = rhs.mElements;
                this->mHolder = rhs.mHolder;
            }
        }
    }
    
    template<typename T>
    void Array<T>::growTo(size_t newCapacity) {
        ukn_assert(!this->mMapped);
        
        T* newArray = ukn_malloc_t(T, newCapacity);
        if(this->mElements) {
            memcpy(newArray, this->mElements, sizeof(T) * this->mCapacity);
        }
        this->mElements = newArray;
        this->mHolder.reset(newArray);
        
        this->mCapacity = newCapacity;
    }
    
    template<typename T>
    void Array<T>::grow() {
        size_t toSize;
        if(this->mCapacity == 0) {
            if(this->mGrow == 0)
                toSize = 1;
            else {
                toSize = this->mGrow;
            }
        } else {
            toSize = this->mCapacity * 2;
        }
        this->growTo(toSize);
    }
    
    template<typename T>
    void Array<T>::append(const T* arr, size_t size) {
        ukn_assert(size > 0);
        
        size_t newSize = this->mSize + size;
        if(newSize >= this->mCapacity) {
            this->growTo(newSize);
        }
        memcpy(this->mElements + this->mSize, arr, size);
        this->mSize = newSize;
    }
    
    template<typename T>
    void Array<T>::move(size_t from, size_t to) {
        ukn_assert(this->mElements);
        ukn_assert(from < this->mSize);
        ukn_assert(!this->mMapped);
        
        if(from == to)
            return;
        
        size_t num = this->mSize - from;
        size_t neededSize = to + num;
        while(neededSize < this->mCapacity) {
            this->grow();
        }
        
        if(from > to) {
            // backward move
            size_t i;
            for(i=0; i<num; ++i) {
                this->mElements[to + i] = this->mElements[from + i];
            }
            
            for(i=(from+i)-1; i<this->mSize; ++i) {
                this->destroy(&(this->mElements[i]));
            }
        } else {
            long i=0;
            for(i=num-1; i>=0; --i) {
                this->mElements[to + i] = this->mElements[from + i];
            }
            
            for(i=int(from); i<int(to); ++i) {
                this->destroy(&(this->mElements[i]));
            }
        }
        
        this->mSize = to + num;
    }
    
    template<typename T>
    void Array<T>::append(const T& elemt) {
        if(this->mSize == this->mCapacity) {
            this->grow();
        }
        ukn_assert(this->mElements);
        this->mElements[this->mSize++] = elemt;
    }
    
    template<typename T>
    void Array<T>::push_back(const T& elemt) {
        this->append(elemt);
    }
    
    template<typename T>
    void Array<T>::pop_back() {
        ukn_assert(this->mElements);
        ukn_assert(this->mSize > 0);
        
        this->destroy(this->mElements[this->mSize-1]);
        this->mSize--;
    }
    
    template<typename T>
    void Array<T>::appendArray(const Array<T>& rhs) {
        size_t size = rhs.size();
        for(size_t i=0; i<size; ++i) {
            this->append(rhs[i]);
        }
    }
    
    template<typename T>
    void Array<T>::reserve(size_t num) {
        ukn_assert(num > 0);
        size_t neededCapacity = this->mSize + num;
        if(neededCapacity > this->mCapacity) {
            this->growTo(neededCapacity);
        }
    }
    
    template<typename T>
    size_t Array<T>::size() const {
        return this->mSize;
    }
    
    template<typename T>
    size_t Array<T>::capacity() const {
        return this->mCapacity;
    }
    
    template<typename T>
    T& Array<T>::operator[](size_t index) const {
        ukn_assert(index < this->mSize);
        ukn_assert(this->mElements);
        
        return this->mElements[index];
    }
    
    template<typename T>
    bool Array<T>::operator==(const Array<T>& rhs) const {
        if(rhs.size() == this->size()) {
            for(size_t i=0; i<this->mSize(); ++i) {
                if(!(this->mElements[i] == rhs.mElements[i]))
                    return false;
            }
            return true;
        }
        return false;
    }
    
    template<typename T>
    bool Array<T>::operator!=(const Array<T>& rhs) const {
        return !(*this == rhs);
    }
    
    template<typename T>
    T& Array<T>::front() const {
        ukn_assert(this->mElements);
        ukn_assert(this->mSize > 0);
        
        return this->mElements[0];
    }
    
    template<typename T>
    T& Array<T>::back() const {
        ukn_assert(this->mElements);
        ukn_assert(this->mSize > 0);
        
        return this->mElements[this->mSize - 1];
    }
    
    template<typename T>
    bool Array<T>::empty() const {
        return this->mSize == 0;
    }
    
    template<typename T>
    typename Array<T>::iterator Array<T>::erase(typename Array<T>::iterator iter) {
        ukn_assert(this->mElements);
        ukn_assert(iter < (this->mElements + this->mSize));
        ukn_assert(iter >= this->mElements);
        ukn_assert(!this->mMapped);
        
        this->erase(uint32(iter - this->mElements));
        return iter;
    }
    
    template<typename T>
    void Array<T>::erase(size_t index) {
        ukn_assert(this->mElements);
        ukn_assert(index < this->mSize);
        ukn_assert(!this->mMapped);
        
        if(index == this->mSize-1) {
            this->destroy(&(this->mElements[index]));
            this->mSize--;
        } else {
            this->move(index+1, index);
        }
    }
    
    template<typename T>
    void Array<T>::insert(size_t index, const T& elemt) {
        ukn_assert(index <= this->mSize);
        ukn_assert(!this->mMapped);
        
        if(index == this->mSize) 
            this->append(elemt);
        else {
            this->move(index, index+1);
            this->mElements[index] = elemt;
        }
    }
    
    template<typename T>
    void Array<T>::pop_front() {
        ukn_assert(this->mElements);
        ukn_assert(this->mSize >= 1);
        ukn_assert(!this->mMapped);
        
        if(this->mSize == 1)
            this->clear();
        else {
            this->move(1, 0);
        }
    }
    
    template<typename T>
    void Array<T>::destroyAll() {
        for(size_t i=0; i<this->mSize; ++i) 
            this->destroy(&(this->mElements[i]));
        this->mSize = 0;
    }
    
    template<typename T>
    void Array<T>::reset() {
        this->mSize = 0;
    }
    
    template<typename T>
    typename Array<T>::const_iterator Array<T>::begin() const {
        return this->mElements;
    }
    
    template<typename T>
    typename Array<T>::const_iterator Array<T>::end() const {
        return this->mElements + this->mSize;
    }
    
    template<typename T>
    typename Array<T>::iterator Array<T>::begin() {
        return this->mElements;
    }
    
    template<typename T>
    typename Array<T>::iterator Array<T>::end() {
        return this->mElements + this->mSize;
    }
    
    template<typename T>
    typename Array<T>::iterator Array<T>::find(const T& elemt) const {
        for (size_t i=0; i<this->mSize; ++i) {
            if(this->mElements[i] == elemt)
                return &(this->mElements[index]);
        }
        return 0;
    }
    
    template<typename T>
    size_t Array<T>::findIndex(const T& elemt) const {
        for(size_t i=0; i<this->mSize; ++i) {
            if(this->mElements[i] == elemt)
                return i;
        }
        return -1;
    }
    
    template<typename T>
    void Array<T>::fill(size_t index, size_t num, const T& elemt) {
        if(index + num > this->mSize)
            this->growTo(index + num);
        
        for(size_t i=0; i<index+num; ++i) {
            this->mElements[i] = elemt;
        }
    }
    
    template<typename T>
    void Array<T>::push_front(const T& elemt) {
        if(this->mSize == 0) {
            grow();
            this->mElements[0] = elemt;
            this->mSize++;
        } else {
            this->move(0, 1);
            this->mElements[0] = elemt;
            this->mSize++;
        }
    }
    
    template<typename T>
    void Array<T>::insertSorted(const T& elm) {
        if(this->mSize == 0) 
            this->append(elm);
        else {
            size_t num = this->mSize;
            size_t half;
            size_t lo = 0;
            size_t hi = this->mSize - 1;
            size_t mid;
            while(lo < hi) {
                if(0 != (half = num/2)) {
                    mid = lo + ((num & 1) ? half : (half - 1));
                    if(elm < this->mElements[mid]) {
                        hi = mid - 1;
                        num = num & 1 ? half : half - 1;
                    } else if(elm > this->mElements[mid]) {
                        lo = mid + 1;
                        num = half;
                    } else {
                        size_t i = mid +1;
                        for(; i<this->mSize; ++i) {
                            if(this->mElements[i] != elm) {
                                this->insert(i, elm);
                                return;
                            }
                        }
                        this->append(elm);
                        return;
                    }
                } else if(0 != num) {
                    if(elm < this->mElements[lo]) {
                        this->insert(lo, elm);
                        return;
                    } else if(elm > this->mElements[lo]) {
                        this->insert(lo+1, elm);
                        return;
                    } else {
                        size_t i = lo +1;
                        for(; i<this->mSize; ++i) {
                            if(this->mElements[i] != elm) {
                                this->insert(i, elm);
                                return;
                            }
                        }
                        this->append(elm);
                        return;
                    }
                    
                } else {
                    ukn_assert(0 == lo);
                    this->insert(lo, elm);
                    return;
                }
            }
            if(elm < this->mElements[lo]) {
                this->insert(lo, elm);
                return;
            } else if(elm > this->mElements[lo]) {
                this->insert(lo+1, elm);
                return;
            } else {
                // can't happen
                ukn_assert(false);
            }
        }
    }
    
    template<typename T>
    bool Array<T>::isSorted() const {
        if(this->mSize > 1) {
            for(size_t i=0; i<this->mSize; ++i) {
                if(this->mElements[i] > this->mElements[i+1]) 
                    return false;
            }
        }
        return true;
    }
    
    template<typename T>
    void Array<T>::sort() const {
        std::sort(this->begin(), this->end());
    }
    
    template<typename T>
    size_t Array<T>::binarySearchIndex(const T& elm) {
        if(this->mSize > 0) {
            size_t num = this->mSize;
            size_t half;
            size_t lo;
            size_t hi = this->mSize - 1;
            size_t mid;
            while(lo < hi) {
                if(0 != (half = num/2)) {
                    mid = lo + ((num & 1) ? half : (half-1));
                    if(elm < this->mElements[mid]) {
                        hi = mid-1;
                        num = num & 1 ? half : half - 1;
                    } else if(elm > this->mElements[mid]) {
                        lo = mid+1;
                        num = half;
                    } else {
                        return mid;
                    }
                } else if(0 != num) {
                    if(elm != this->mElements[lo]) 
                        return InvalidIndex;
                    else 
                        return lo;
                } else 
                    break;
            }
        }
        return InvalidIndex;
    }
    
    template<typename T, size_t Size>
    class FixedArray {
    public:
        FixedArray();
        FixedArray(const T& initialValue);
        
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef T& reference;
        typedef const T& const_reference;
        
        reference front();
        reference back();
        
        iterator begin();
        iterator end();
        
        const_reference front() const;
        const_reference back() const;
        
        const_iterator begin() const;
        const_iterator end() const;
        
        enum {
            ArraySize = Size
        };
        
        size_t size() const;
        
        reference operator[](size_t index);
        const_reference operator[](size_t index) const;
        
    private:
        size_t mSize;
        T* mElements;
    };
    
    template<typename T, size_t Size>
    FixedArray<T, Size>::FixedArray():
    mSize(ArraySize) {
        ukn_assert(Size != 0);
        mElements = ukn_malloc_t(T, Size);
        ukn_assert(mElements);
    }
    
    template<typename T, size_t Size>
    FixedArray<T, Size>::FixedArray(const T& initialValue):
    mSize(ArraySize) {
        ukn_assert(Size != 0);
        mElements = ukn_malloc_t(T, Size);
        ukn_assert(mElements);
        
        for(size_t i=0; i<mSize; ++i)
            mElements[i] = initialValue;
    }
    
    template<typename T, size_t Size>
    typename FixedArray<T, Size>::reference FixedArray<T, Size>::front() {
        ukn_assert(mElements != 0);
        return mElements[0];
    }
    
    template<typename T, size_t Size>
    typename FixedArray<T, Size>::reference FixedArray<T, Size>::back() {
        ukn_assert(mElements != 0);
        return mElements[mSize-1];
    }
    
    template<typename T, size_t Size>
    typename FixedArray<T, Size>::const_reference FixedArray<T, Size>::front() const {
        ukn_assert(mElements != 0);
        return mElements[0];
    }
    
    template<typename T, size_t Size>
    typename FixedArray<T, Size>::const_reference FixedArray<T, Size>::back() const {
        ukn_assert(mElements != 0);
        return mElements[mSize-1];
    }
    
    template<typename T, size_t Size>
    typename FixedArray<T, Size>::iterator FixedArray<T, Size>::begin() {
        ukn_assert(mElements != 0);
        return &mElements[0];
    }
    
    template<typename T, size_t Size>
    typename FixedArray<T, Size>::iterator FixedArray<T, Size>::end() {
        ukn_assert(mElements != 0);
        return &mElements[mSize-1];
    }
    
    template<typename T, size_t Size>
    typename FixedArray<T, Size>::const_iterator FixedArray<T, Size>::begin() const {
        ukn_assert(mElements != 0);
        return &mElements[0];
    }
    
    template<typename T, size_t Size>
    typename FixedArray<T, Size>::const_iterator FixedArray<T, Size>::end() const {
        ukn_assert(mElements != 0);
        return &mElements[mSize-1];
    }
    
    template<typename T, size_t Size>
    size_t FixedArray<T, Size>::size() const {
        return mSize;
    }
    
    template<typename T, size_t Size>
    typename FixedArray<T, Size>::reference FixedArray<T, Size>::operator[](size_t index) {
        ukn_assert(mElements != 0);
        ukn_assert(index < mSize);
        return mElements[index];
    }
    
    template<typename T, size_t Size>
    typename FixedArray<T, Size>::const_reference FixedArray<T, Size>::operator[](size_t index) const {
        ukn_assert(mElements != 0);
        ukn_assert(index < mSize);
        return mElements[index];
    }
    
    template<typename Key, typename Value>
    class Pair {
    public:
        Pair() { }
        
        Pair(const Key& key, const Value& value):
        mKey(key),
        mValue(value) {
            
        }
        
        Pair(const Key& key):
        mKey(key) {
            
        }
        
        Key& key()      { return this->mKey; }
        Value& value()  { return this->mValue; }
        
        Key& first()    { return this->mKey; }
        Value& second() { return this->mValue; }
        
        const Key& key() const      { return this->mKey; }
        const Value& value() const  { return this->mValue; }
        
        const Key& first() const    { return this->mKey; }
        const Value& second() const { return this->mValue; }
        
        typedef Key KeyType;
        typedef Value ValueType;
        typedef Pair<Key, Value> SelfType;
        
        bool operator == (const SelfType& rhs) const { return this->mKey == rhs.mKey && this->mValue == rhs.mValue; }
        bool operator != (const SelfType& rhs) const { return this->mKey != rhs.mKey; }
        bool operator <= (const SelfType& rhs) const { return this->mKey <= rhs.mKey; }
        bool operator >= (const SelfType& rhs) const { return this->mKey >= rhs.mKey; }
        bool operator < (const SelfType& rhs) const { return this->mKey < rhs.mKey; }
        bool operator > (const SelfType& rhs) const { return this->mKey > rhs.mKey; }
        
        Pair& operator=(const SelfType& rhs) {
            this->mKey = rhs.mKey;
            this->mValue = rhs.mValue;
        }
        
    private:
        Key mKey;
        Value mValue;
    };
    
    template<typename Key, typename Value>
    class Dictionary {
    public:
        typedef Key KeyType;
        typedef Value ValueType;
        typedef Pair<Key, Value> PairType;
        typedef Array<PairType> DictionaryType;
        typedef Dictionary<Key, Value> SelfType;
        
        typedef typename DictionaryType::iterator iterator;
        typedef typename DictionaryType::const_iterator const_iterator;
        typedef PairType reference;
        typedef const PairType const_reference;
        
        Dictionary();
        Dictionary(const SelfType& rhs);
        
        Dictionary& operator=(const SelfType& rhs);
        
        void add(const Key& key, const Value& val);
        void del(const Key& key);
        
        void insert(const PairType& pair);
        void insert(const Key& key, const Value& val);
        void remove(const Key& key);
        
        void push_back(const PairType& pair);
        
        void erase(iterator iter);
        
        iterator begin();
        iterator end();
        
        const_iterator begin() const;
        const_iterator end() const;
        
        iterator find(const Key& key) const;
        
        PairType& front() const;
        PairType& back() const;
        
        ValueType& get(const Key& key);
        const ValueType& get(const Key& key) const;
        
        ValueType& at(const Key& key);
        const ValueType& at(const Key& key) const;
        
        ValueType& operator[](const Key& key);
        ValueType& operator[](uint32 index);
        const ValueType& operator[](const Key& key) const;
        const ValueType& operator[](uint32 index) const;
        
        uint32 getIndex(const Key& key);
        ValueType& atIndex(uint32 index) const;
        
        bool contains(const Key& key); 
        
        void beginBulkInsert();
        void endBulkInsert();
        
        Array<Value> valueAsArray() const;
        Array<Key> keyAsArray() const;
        
    private:
        bool mIsBulkInsert;
        
        DictionaryType mDictionary;
    };
    
    template<typename Key, typename Value>
    Dictionary<Key, Value>::Dictionary():
    mIsBulkInsert(false) {
        
    }
    
    template<typename Key, typename Value>
    Dictionary<Key, Value>::Dictionary(const SelfType& rhs):
    mIsBulkInsert(false),
    mDictionary(rhs.mDictionary) {
        ukn_assert(!rhs.mIsBulkInsert);
    }
    
    template<typename Key, typename Value>
    Dictionary<Key, Value>& Dictionary<Key, Value>::operator=(const SelfType& rhs) {
        ukn_assert(!rhs.mIsBulkInsert);
        
        this->mDictionary = rhs.mDictionary;
        return *this;
    }
    
    template<typename Key, typename Value>
    void Dictionary<Key, Value>::insert(const PairType& pair) {
        if(!mIsBulkInsert)
            this->mDictionary.insertSorted(pair);
        else
            this->mDictionary.insert(pair);
    }
    
    template<typename Key, typename Value>
    void Dictionary<Key, Value>::add(const Key& key, const Value& val) {
        if(!mIsBulkInsert)
            this->mDictionary.insertSorted(PairType(key, val));
        else
            this->mDictionary.insert(PairType(key, val));
    }
    
    template<typename Key, typename Value>
    void Dictionary<Key, Value>::del(const Key& key) {
        uint32 index = this->mDictionary.findIndex(key);
        if(index != DictionaryType::InvalidIndex) {
            this->mDictionary.erase(index);
        }
    }
    
    template<typename Key, typename Value>
    void Dictionary<Key, Value>::insert(const Key& key, const Value& val) {
        if(!mIsBulkInsert)
            this->mDictionary.insertSorted(PairType(key, val));
        else
            this->mDictionary.insert(PairType(key, val));
    }
    
    template<typename Key, typename Value>
    void Dictionary<Key, Value>::remove(const Key& key) {
        uint32 index = this->mDictionary.findIndex(key);
        if(index != DictionaryType::InvalidIndex) {
            this->mDictionary.erase(index);
        }
    }
    
    template<typename Key, typename Value>
    void Dictionary<Key, Value>::push_back(const PairType& pair) {
        if(!mIsBulkInsert)
            this->mDictionary.insertSorted(pair);
        else
            this->mDictionary.insert(pair);
    }
    
    template<typename Key, typename Value>
    void Dictionary<Key, Value>::erase(iterator iter) {
        this->mDictionary.erase(iter);
    }
    
    template<typename Key, typename Value>
    typename Dictionary<Key, Value>::iterator Dictionary<Key, Value>::begin() {
        return this->mDictionary.begin();
    }
    
    template<typename Key, typename Value>
    typename Dictionary<Key, Value>::iterator Dictionary<Key, Value>::end() {
        return this->mDictionary.end();
    }
    
    template<typename Key, typename Value>
    typename Dictionary<Key, Value>::const_iterator Dictionary<Key, Value>::begin() const {
        return this->mDictionary.begin();
    }
    
    template<typename Key, typename Value>
    typename Dictionary<Key, Value>::const_iterator Dictionary<Key, Value>::end() const {
        return this->mDictionary.end();
    }
    
    template<typename Key, typename Value>
    typename Dictionary<Key, Value>::PairType& Dictionary<Key, Value>::front() const {
        return this->mDictionary.front();
    }
    
    template<typename Key, typename Value>
    typename Dictionary<Key, Value>::PairType& Dictionary<Key, Value>::back() const {
        return this->mDictionary.front();
    }
    
    template<typename Key, typename Value>
    typename Dictionary<Key, Value>::ValueType& Dictionary<Key, Value>::get(const Key& key) {
        uint32 index = this->mDictionary.findIndex(key);
        ukn_assert(index != DictionaryType::InvalidIndex);
        
        return this->mDictionary[index];
    }
    
    template<typename Key, typename Value>
    const typename Dictionary<Key, Value>::ValueType& Dictionary<Key, Value>::get(const Key& key) const {
        uint32 index = this->mDictionary.findIndex(key);
        ukn_assert(index != DictionaryType::InvalidIndex);
        
        return this->mDictionary[index];
    }
    
    template<typename Key, typename Value>
    typename Dictionary<Key, Value>::ValueType& Dictionary<Key, Value>::at(const Key& key) {
        uint32 index = this->mDictionary.findIndex(key);
        ukn_assert(index != DictionaryType::InvalidIndex);
        
        return this->mDictionary[index];
    }
    
    template<typename Key, typename Value>
    const typename Dictionary<Key, Value>::ValueType& Dictionary<Key, Value>::at(const Key& key) const {
        uint32 index = this->mDictionary.findIndex(key);
        ukn_assert(index != DictionaryType::InvalidIndex);
        
        return this->mDictionary[index];
    }
    
    template<typename Key, typename Value>
    const typename Dictionary<Key, Value>::ValueType& Dictionary<Key, Value>::operator[](const Key& key) const {
        uint32 index = this->mDictionary.findIndex(key);
        ukn_assert(index != DictionaryType::InvalidIndex);
        
        return this->mDictionary[index];
    }
    
    template<typename Key, typename Value>
    const typename Dictionary<Key, Value>::ValueType& Dictionary<Key, Value>::operator[](uint32 index) const {
        ukn_assert(index != DictionaryType::InvalidIndex);
        
        return this->mDictionary[index];
    }
    
    template<typename Key, typename Value>
    typename Dictionary<Key, Value>::ValueType& Dictionary<Key, Value>::operator[](const Key& key) {
        uint32 index = this->mDictionary.findIndex(key);
        ukn_assert(index != DictionaryType::InvalidIndex);
        
        return this->mDictionary[index];
    }
    
    template<typename Key, typename Value>
    typename Dictionary<Key, Value>::ValueType& Dictionary<Key, Value>::operator[](uint32 index) {
        ukn_assert(index != DictionaryType::InvalidIndex);
        
        return this->mDictionary[index]; 
    }
    
    template<typename Key, typename Value>
    uint32 Dictionary<Key, Value>::getIndex(const Key& key) {
        uint32 index = this->mDictionary.findIndex(key);
        return index;
    }
    
    template<typename Key, typename Value>
    typename Dictionary<Key, Value>::ValueType& Dictionary<Key, Value>::atIndex(uint32 index) const {
        ukn_assert(index != DictionaryType::InvalidIndex);
        
        return this->mDictionary[index];
    }
    
    template<typename Key, typename Value>
    bool Dictionary<Key, Value>::contains(const Key& key) {
        uint32 index = this->mDictionary.findIndex(key);
        return (index != DictionaryType::InvalidIndex);
    }
    
    template<typename Key, typename Value>
    typename Dictionary<Key, Value>::iterator Dictionary<Key, Value>::find(const Key& key) const {
        return this->mDictionary.find(key);
    }
    
    template<typename Key, typename Value>
    void Dictionary<Key, Value>::beginBulkInsert() {
        this->mIsBulkInsert = true;
    }
    
    template<typename Key, typename Value>
    void Dictionary<Key, Value>::endBulkInsert() {
        this->mIsBulkInsert = false;
        this->mDictionary.sort();
    }
    
    template<typename Key, typename Value>
    Array<Value> Dictionary<Key, Value>::valueAsArray() const {
        Array<Value> array;
        iterator it = this->begin();
        iterator end = this>end();
        for(; it != end; ++it) 
            array.insert(it->second());
        return array;
    }
    
    template<typename Key, typename Value>
    Array<Key> Dictionary<Key, Value>::keyAsArray() const {
        Array<Value> array;
        iterator it = this->begin();
        iterator end = this>end();
        for(; it != end; ++it) 
            array.insert(it->first());
        return array;
    }
    
    namespace tuple {
        
        struct null_type;
        
    } // namespace tuple
    
    template<typename T1, typename T2, typename T3, typename T4, typename T5>
    struct SimpleTuple;
    
    template<typename T>
    struct SimpleTuple<T, tuple::null_type, tuple::null_type, tuple::null_type, tuple::null_type> {
        SimpleTuple() { }
        
        SimpleTuple(T a0):
        mA0(a0) {
            
        }
        
        typedef SimpleTuple<T, tuple::null_type, tuple::null_type, tuple::null_type, tuple::null_type> SelfType;
        
        T get0() { return this->mA0; }
        
        template<typename AT>
        bool operator=(const SimpleTuple<AT, tuple::null_type, tuple::null_type, tuple::null_type, tuple::null_type>& rhs) {
            this->mA0 = rhs.mA0;
        }
        
        T mA0;
    };
    
    template<typename T, typename T1>
    struct SimpleTuple<T, T1, tuple::null_type, tuple::null_type, tuple::null_type> {
        SimpleTuple() { }
        
        SimpleTuple(T a0, T1 a1):
        mA0(a0),
        mA1(a1) {
            
        }
        
        typedef SimpleTuple<T, T1, tuple::null_type, tuple::null_type, tuple::null_type> SelfType;
        
        T get0() { return this->mA0; }
        T1 get1() { return this->mA1; }
        
        template<typename AT, typename AT1>
        bool operator=(const SimpleTuple<AT, AT1, tuple::null_type, tuple::null_type, tuple::null_type>& rhs) {
            this->mA0 = rhs.mA0;
            this->mA1 = rhs.mA1;
        }
        
        T mA0;
        T1 mA1;
    };
    
    template<typename T, typename T1, typename T2>
    struct SimpleTuple<T, T1, T2, tuple::null_type, tuple::null_type> {
        SimpleTuple() { }
        
        SimpleTuple(T a0, T1 a1, T2 a2):
        mA0(a0),
        mA1(a1),
        mA2(a2) {
            
        }
        
        typedef SimpleTuple<T, T1, T2, tuple::null_type, tuple::null_type> SelfType;
        
        T get0() { return this->mA0; }
        T1 get1() { return this->mA1; }
        T2 get2() { return this->mA2; }
        
        template<typename AT, typename AT1, typename AT2>
        bool operator=(const SimpleTuple<AT, AT1, AT2, tuple::null_type, tuple::null_type>& rhs) {
            this->mA0 = rhs.mA0;
            this->mA1 = rhs.mA1;
            this->mA2 = rhs.mA2;
        }
        
        T mA0;
        T1 mA1;
        T2 mA2;
    };
    
    template<typename T, typename T1, typename T2, typename T3>
    struct SimpleTuple<T, T1, T2, T3, tuple::null_type> {
        SimpleTuple() { }
        
        SimpleTuple(T a0, T1 a1, T2 a2, T3 a3):
        mA0(a0),
        mA1(a1),
        mA2(a2),
        mA3(a3) {
            
        }
        
        typedef SimpleTuple<T, T1, T2, T3, tuple::null_type> SelfType;
        
        T get0() { return this->mA0; }
        T1 get1() { return this->mA1; }
        T2 get2() { return this->mA2; }
        T3 get3() { return this->mA3; }
        
        template<typename AT, typename AT1, typename AT2, typename AT3>
        bool operator=(const SimpleTuple<AT, AT1, AT2, AT3, tuple::null_type>& rhs) {
            this->mA0 = rhs.mA0;
            this->mA1 = rhs.mA1;
            this->mA2 = rhs.mA2;
            this->mA3 = rhs.mA3;
        }
        
        T mA0;
        T1 mA1;
        T2 mA2;
        T3 mA3;
    };
    
    template<typename T, typename T1, typename T2, typename T3, typename T4>
    struct SimpleTuple {
        SimpleTuple() { }
        
        SimpleTuple(T a0, T1 a1, T2 a2, T3 a3, T4 a4):
        mA0(a0),
        mA1(a1),
        mA2(a2),
        mA3(a3),
        mA4(a4) {
            
        }
        
        typedef SimpleTuple<T, T1, T2, T3, T4> SelfType;
        
        T get0() { return this->mA0; }
        T1 get1() { return this->mA1; }
        T2 get2() { return this->mA2; }
        T3 get3() { return this->mA3; }
        T4 get4() { return this->mA4; }
        
        template<typename AT, typename AT1, typename AT2, typename AT3, typename AT4>
        bool operator=(const SimpleTuple<AT, AT1, AT2, AT3, AT4>& rhs) {
            this->mA0 = rhs.mA0;
            this->mA1 = rhs.mA1;
            this->mA2 = rhs.mA2;
            this->mA3 = rhs.mA3;
            this->mA4 = rhs.mA4;
        }
        
        T mA0;
        T1 mA1;
        T2 mA2;
        T3 mA3;
        T4 mA4;
    };
    
    template<int Count, typename Type>
    struct PointTemplate {
        
        typedef PointTemplate<Count, Type> SelfType;
        typedef Type PointType;
        
        enum {
            PointCount = Count,
        };
        
        ukn_static_assert(Count > 1);
        
        template<typename T2>
        SelfType operator+(const PointTemplate<Count, T2>& rhs) const {
            SelfType tmp = *this;
            for(int i=0; i<PointCount; ++i) {
                tmp.value[i] += static_cast<Type>(rhs.value[i]);
            }
            return tmp;
        }
        
        template<typename T2>
        SelfType operator-(const PointTemplate<Count, T2>& rhs) const {
            SelfType tmp = *this;
            for(int i=0; i<PointCount; ++i) {
                tmp.value[i] -= static_cast<Type>(rhs.value[i]);
            }
            return tmp;
        }
        
        template<typename T2>
        SelfType& operator=(const PointTemplate<Count, T2>& rhs) {
            for(int i=0; i<PointCount; ++i) {
                this->value[i] = static_cast<Type>(rhs.value[i]);
            }
            return *this;
        }
        
        template<typename T2>
        bool operator==(const PointTemplate<Count, T2>& rhs) {
            for(int i=0; i<PointCount; ++i) {
                if(this->value[i] != static_cast<Type>(rhs.value[i]))
                    return false;
            }
            return true;
        }
        
        template<typename T2>
        bool operator!=(const PointTemplate<Count, T2>& rhs) {
            return !(*this == rhs);
        }
        
        template<typename T2>
        SelfType& operator+=(const PointTemplate<Count, T2>& rhs) {
            for(int i=0; i<PointCount; ++i) {
                this->value[i] += static_cast<Type>(rhs.value[i]);
            }
            return *this;
        }
        
        template<typename T2>
        SelfType& operator-=(const PointTemplate<Count, T2>& rhs) {
            SelfType tmp = *this;
            for(int i=0; i<PointCount; ++i) {
                this->value[i] -= static_cast<Type>(rhs.value[i]);
            }
            return *this;
        }
        
        Type& operator[](uint32 index) {
            ukn_assert(index >= 0 && index < PointCount);
            return this->value[index];
        }
        
        const Type& operator[](uint32 index) const {
            ukn_assert(index >= 0 && index < PointCount);
            return this->value[index];
        }
        
        
        Type value[Count];
    };
    
    
#define UKN_POINT_TEMP_PARAM(n, d) d UKN_CAT(value, n)
#define UKN_POINT_TEMP_PARAM_ARGS(n, d) UKN_ENUM_N(n, UKN_POINT_TEMP_PARAM, d) 
#define UKN_POINT_PARAM_EXPAND(n, d) d[n] = d##n;
    
#define DEF_POINT_TEMPLATE_N(n, Type) \
    template<>                                         \
    struct PointTemplate<n, Type> {                             \
        typedef PointTemplate<n, Type> SelfType;                \
        typedef Type PointType;                                     \
        enum {                                                      \
            PointCount = n,                                         \
        };                                                          \
        PointTemplate() {                                       \
        }                                                           \
        PointTemplate(UKN_POINT_TEMP_PARAM_ARGS(n, Type)) {     \
            UKN_REPEAT_N(n, UKN_POINT_PARAM_EXPAND, value)        \
        } \
        template<typename T2>                                           \
        SelfType operator+(const PointTemplate<n, T2>& rhs) const { \
            SelfType tmp = *this;                                       \
            for(int i=0; i<PointCount; ++i) {                           \
                tmp.value[i] += static_cast<Type>(rhs.value[i]);        \
            }                                                           \
            return tmp;                                                 \
        }                                                               \
        template<typename T2>                                           \
        SelfType operator-(const PointTemplate<n, T2>& rhs) const { \
            SelfType tmp = *this;                                       \
            for(int i=0; i<PointCount; ++i) {                           \
                tmp.value[i] -= static_cast<Type>(rhs.value[i]);        \
            }\
            return tmp;\
        }\
        template<typename T2>\
        SelfType& operator=(const PointTemplate<n, T2>& rhs) {\
            for(int i=0; i<PointCount; ++i) {\
                this->value[i] = static_cast<Type>(rhs.value[i]);\
            }\
            return *this;\
        }\
        template<typename T2>\
        bool operator==(const PointTemplate<n, T2>& rhs) {\
            for(int i=0; i<PointCount; ++i) {\
                if(this->value[i] != static_cast<Type>(rhs.value[i]))\
                    return false;\
            }\
            return true;\
        }\
        template<typename T2>\
        bool operator!=(const PointTemplate<n, T2>& rhs) {\
            return !(*this == rhs);\
        }\
        template<typename T2>\
        SelfType& operator+=(const PointTemplate<n, T2>& rhs) {\
            for(int i=0; i<PointCount; ++i) {\
                this->value[i] += static_cast<Type>(rhs.value[i]);\
            }\
            return *this;\
        }\
        template<typename T2>\
        SelfType& operator-=(const PointTemplate<n, T2>& rhs) {\
            SelfType tmp = *this;\
            for(int i=0; i<PointCount; ++i) {\
                this->value[i] -= static_cast<Type>(rhs.value[i]);\
            }\
            return *this;\
        }\
        Type& operator[](uint32 index) {\
            ukn_assert(index >= 0 && index < PointCount);\
            return this->value[index];\
        }\
        const Type& operator[](uint32 index) const {\
            ukn_assert(index >= 0 && index < PointCount);\
            return this->value[index];\
        }\
        Type value[n];\
    };
    
    DEF_POINT_TEMPLATE_N(1, float);
    DEF_POINT_TEMPLATE_N(2, float);
    DEF_POINT_TEMPLATE_N(3, float);
    DEF_POINT_TEMPLATE_N(4, float);
    
    DEF_POINT_TEMPLATE_N(1, int32);
    DEF_POINT_TEMPLATE_N(2, int32);
    DEF_POINT_TEMPLATE_N(3, int32);
    DEF_POINT_TEMPLATE_N(4, int32);
    
    DEF_POINT_TEMPLATE_N(1, uint32);
    DEF_POINT_TEMPLATE_N(2, uint32);
    DEF_POINT_TEMPLATE_N(3, uint32);
    DEF_POINT_TEMPLATE_N(4, uint32);
    
#undef DEF_POINT_TEMPLATE_N
#undef UKN_POINT_TEMP_PARAM
#undef UKN_POINT_TEMP_PARAM_ARGS
#undef UKN_POINT_PARAM_EXPAND
    
    class RefCounted {
    public:
        RefCounted(): mCounter(1) {}
        
        void duplicate() const;
        void release() const;
        int refCount() const;
        
    protected:
        virtual ~RefCounted() {}
        
    private:
        mutable int mCounter;
    };
    
    inline int RefCounted::refCount() const {
        return this->mCounter;
    }
    
    inline void RefCounted::duplicate() const {
        ++this->mCounter;
    }
    
    inline void RefCounted::release() const {
        if(--this->mCounter == 0)
            delete this;
    }
    
    class FourCC {
    public:
        FourCC();
        FourCC(uint32 f);
        FourCC(const ukn_string& s);
        
        bool operator==(const FourCC& rhs) const;
        bool operator!=(const FourCC& rhs) const;
        bool operator<(const FourCC& rhs) const;
        bool operator>(const FourCC& rhs) const;
        bool operator<=(const FourCC& rhs) const;
        bool operator>=(const FourCC& rhs) const;
        
        bool isValid() const;
        void fromUInt(uint32 f);
        uint32 asUInt() const;
        
        void fromString(const ukn_string& s) {
            ukn_assert(!s.empty() &&
                       s.size() >= 4);
            
            this->mFourCC = uint32(s[0] | s[1]<<8 | s[2]<<16 | s[3]<<24);
        }
        ukn_string toString() const {
            ukn_assert(isValid());
            
            std::string str("1234");
            str[0] = static_cast<char>((this->mFourCC & 0xFF000000) >> 24);
            str[1] = static_cast<char>((this->mFourCC & 0x00FF0000) >> 16);
            str[2] = static_cast<char>((this->mFourCC & 0x0000FF00) >> 8);
            str[3] = static_cast<char>((this->mFourCC & 0x000000FF));
            return str;
        }
        
        static ukn_string ToString(const FourCC& f);
        static FourCC FromString(const ukn_string& s);
        
    private:
        uint32 mFourCC;
    };
    
    inline FourCC::FourCC():
    mFourCC(0) {
        
    }
    
    inline FourCC::FourCC(uint32 f):
    mFourCC(f) {
        
    }
    
    inline FourCC::FourCC(const ukn_string& f) {
        this->fromString(f);
    }
    
    inline bool FourCC::operator==(const FourCC& rhs) const {
        return this->mFourCC == rhs.mFourCC;
    }
    
    inline bool FourCC::operator!=(const FourCC& rhs) const {
        return this->mFourCC != rhs.mFourCC;
    }
    
    inline bool FourCC::operator<(const FourCC& rhs) const {
        return this->mFourCC < rhs.mFourCC;
    }
    
    inline bool FourCC::operator>(const FourCC& rhs) const {
        return this->mFourCC > rhs.mFourCC;
    }
    
    inline bool FourCC::operator<=(const FourCC& rhs) const {
        return this->mFourCC <= rhs.mFourCC;
    }
    
    inline bool FourCC::operator>=(const FourCC& rhs) const {
        return this->mFourCC >= rhs.mFourCC;
    }
    
    inline bool FourCC::isValid() const {
        return this->mFourCC != 0;
    }
    
    inline void FourCC::fromUInt(uint32 i) {
        this->mFourCC = i;
    }
    
    inline uint32 FourCC::asUInt() const {
        return this->mFourCC;
    }
    
    inline FourCC FourCC::FromString(const ukn_string& s) {
        FourCC cc;
        cc.fromString(s);
        return cc;
    }
    
    inline ukn_string FourCC::ToString(const FourCC& rhs) {
        return rhs.toString();
    }
    
    template<typename T>
    class ConstVectorIterator {
    public:        
        typedef typename T::const_iterator ConstIteratorType;
        typedef typename T::value_type ValueType;
        
        ConstVectorIterator(const T& container):
        mCurr(container.begin()),
        mEnd(container.end()) {
            
        }
        
        ConstVectorIterator(ConstIteratorType start, ConstIteratorType end):
        mCurr(start),
        mEnd(end) {
            
        }
        
        bool hasMoreElements() {
            return mCurr != mEnd;
        }
        
        ValueType getNext() {
            return *mCurr++;
        }
        
        const ValueType& getNextRef() {
            return *mCurr++;
        }
        
        ValueType peekNext() {
            return *mCurr;
        }
        
        void moveNext() {
            mCurr++;
        }
        
    private:
        ConstIteratorType mEnd;
        ConstIteratorType mCurr;
    };
    
    template<typename T>
    class VectorIterator {
    public:        
        typedef typename T::iterator IteratorType;
        typedef typename T::value_type ValueType;
        
        VectorIterator(const T& container):
        mCurr(container.begin()),
        mEnd(container.end()) {
            
        }
        
        VectorIterator(IteratorType start, IteratorType end):
        mCurr(start),
        mEnd(end) {
            
        }
        
        bool hasMoreElements() {
            return mCurr != mEnd;
        }
        
        ValueType getNext() {
            return *mCurr++;
        }
        
        const ValueType& getNextRef() {
            return *mCurr++;
        }
        
        ValueType peekNext() {
            return *mCurr;
        }
        
        void moveNext() {
            mCurr++;
        }
        
    private:
        IteratorType mEnd;
        IteratorType mCurr;
    };
    
} // namespace ukn

#endif