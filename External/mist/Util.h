//
//  Util.h
//  Mist
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTUtil_h
#define MISTUtil_h

#include "mist/Platform.h"
#include "mist/Ptr.h"
#include "mist/Preprocessor.h"
#include "mist/Basic.h"
#include "mist/Interfaces.h"

#include <algorithm>
#include <list>
#include <vector>

namespace mist {


    template<typename T>
    struct Array: public std::vector<T> {
        
    };

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
        mist_assert(Size != 0);
        mElements = mist_malloc_t(T, Size);
        mist_assert(mElements);
    }

    template<typename T, size_t Size>
    FixedArray<T, Size>::FixedArray(const T& initialValue):
    mSize(ArraySize) {
        mist_assert(Size != 0);
        mElements = mist_malloc_t(T, Size);
        mist_assert(mElements);

        for(size_t i=0; i<mSize; ++i)
            mElements[i] = initialValue;
    }

    template<typename T, size_t Size>
    typename FixedArray<T, Size>::reference FixedArray<T, Size>::front() {
        mist_assert(mElements != 0);
        return mElements[0];
    }

    template<typename T, size_t Size>
    typename FixedArray<T, Size>::reference FixedArray<T, Size>::back() {
        mist_assert(mElements != 0);
        return mElements[mSize-1];
    }

    template<typename T, size_t Size>
    typename FixedArray<T, Size>::const_reference FixedArray<T, Size>::front() const {
        mist_assert(mElements != 0);
        return mElements[0];
    }

    template<typename T, size_t Size>
    typename FixedArray<T, Size>::const_reference FixedArray<T, Size>::back() const {
        mist_assert(mElements != 0);
        return mElements[mSize-1];
    }

    template<typename T, size_t Size>
    typename FixedArray<T, Size>::iterator FixedArray<T, Size>::begin() {
        mist_assert(mElements != 0);
        return &mElements[0];
    }

    template<typename T, size_t Size>
    typename FixedArray<T, Size>::iterator FixedArray<T, Size>::end() {
        mist_assert(mElements != 0);
        return &mElements[mSize-1];
    }

    template<typename T, size_t Size>
    typename FixedArray<T, Size>::const_iterator FixedArray<T, Size>::begin() const {
        mist_assert(mElements != 0);
        return &mElements[0];
    }

    template<typename T, size_t Size>
    typename FixedArray<T, Size>::const_iterator FixedArray<T, Size>::end() const {
        mist_assert(mElements != 0);
        return &mElements[mSize-1];
    }

    template<typename T, size_t Size>
    size_t FixedArray<T, Size>::size() const {
        return mSize;
    }

    template<typename T, size_t Size>
    typename FixedArray<T, Size>::reference FixedArray<T, Size>::operator[](size_t index) {
        mist_assert(mElements != 0);
        mist_assert(index < mSize);
        return mElements[index];
    }

    template<typename T, size_t Size>
    typename FixedArray<T, Size>::const_reference FixedArray<T, Size>::operator[](size_t index) const {
        mist_assert(mElements != 0);
        mist_assert(index < mSize);
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
            if(this != &rhs) {
                this->mKey = rhs.mKey;
                this->mValue = rhs.mValue;
            }
            return *this;
        }

    private:
        Key mKey;
        Value mValue;
    };

    template<typename Key, typename Val>
    static Pair<Key, Val> make_pair(const Key& key, const Val& val) {
        return Pair<Key, Val>(key, val);
    }

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
        mist_assert(!rhs.mIsBulkInsert);
    }

    template<typename Key, typename Value>
    Dictionary<Key, Value>& Dictionary<Key, Value>::operator=(const SelfType& rhs) {
        mist_assert(!rhs.mIsBulkInsert);

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
        mist_assert(index != DictionaryType::InvalidIndex);

        return this->mDictionary[index];
    }

    template<typename Key, typename Value>
    const typename Dictionary<Key, Value>::ValueType& Dictionary<Key, Value>::get(const Key& key) const {
        uint32 index = this->mDictionary.findIndex(key);
        mist_assert(index != DictionaryType::InvalidIndex);

        return this->mDictionary[index];
    }

    template<typename Key, typename Value>
    typename Dictionary<Key, Value>::ValueType& Dictionary<Key, Value>::at(const Key& key) {
        uint32 index = this->mDictionary.findIndex(key);
        mist_assert(index != DictionaryType::InvalidIndex);

        return this->mDictionary[index];
    }

    template<typename Key, typename Value>
    const typename Dictionary<Key, Value>::ValueType& Dictionary<Key, Value>::at(const Key& key) const {
        uint32 index = this->mDictionary.findIndex(key);
        mist_assert(index != DictionaryType::InvalidIndex);

        return this->mDictionary[index];
    }

    template<typename Key, typename Value>
    const typename Dictionary<Key, Value>::ValueType& Dictionary<Key, Value>::operator[](const Key& key) const {
        uint32 index = this->mDictionary.findIndex(key);
        mist_assert(index != DictionaryType::InvalidIndex);

        return this->mDictionary[index];
    }

    template<typename Key, typename Value>
    const typename Dictionary<Key, Value>::ValueType& Dictionary<Key, Value>::operator[](uint32 index) const {
        mist_assert(index != DictionaryType::InvalidIndex);

        return this->mDictionary[index];
    }

    template<typename Key, typename Value>
    typename Dictionary<Key, Value>::ValueType& Dictionary<Key, Value>::operator[](const Key& key) {
        uint32 index = this->mDictionary.findIndex(key);
        mist_assert(index != DictionaryType::InvalidIndex);

        return this->mDictionary[index];
    }

    template<typename Key, typename Value>
    typename Dictionary<Key, Value>::ValueType& Dictionary<Key, Value>::operator[](uint32 index) {
        mist_assert(index != DictionaryType::InvalidIndex);

        return this->mDictionary[index];
    }

    template<typename Key, typename Value>
    uint32 Dictionary<Key, Value>::getIndex(const Key& key) {
        uint32 index = this->mDictionary.findIndex(key);
        return index;
    }

    template<typename Key, typename Value>
    typename Dictionary<Key, Value>::ValueType& Dictionary<Key, Value>::atIndex(uint32 index) const {
        mist_assert(index != DictionaryType::InvalidIndex);

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

        struct null_type {};

    } // namespace tuple

    template<typename T1, typename T2, typename T3, typename T4, typename T5>
    struct Tuple;
    
     typedef Tuple<tuple::null_type,
              tuple::null_type,
              tuple::null_type,
              tuple::null_type,
              tuple::null_type> EmptyTuple;


    template<>
     struct Tuple<tuple::null_type, tuple::null_type, tuple::null_type, tuple::null_type, tuple::null_type> {
        Tuple() { }

        bool empty() const { return true; }
        tuple::null_type get0() const { return tuple::null_type(); };
        EmptyTuple tail() const { return EmptyTuple(); }
     };

    template<typename T>
    struct Tuple<T, tuple::null_type, tuple::null_type, tuple::null_type, tuple::null_type> {
        Tuple() { }

        Tuple(T a0):
        mA0(a0) {

        }
        bool empty() const { return false; }
        EmptyTuple tail() const { return EmptyTuple(); }

        typedef Tuple<T, tuple::null_type, tuple::null_type, tuple::null_type, tuple::null_type> SelfType;

        T get0() const { return this->mA0; }

        template<typename AT>
        Tuple& operator=(const Tuple<AT, tuple::null_type, tuple::null_type, tuple::null_type, tuple::null_type>& rhs) {
            this->mA0 = rhs.mA0;
            return *this;
        }

        T mA0;
    };

    template<typename T, typename T1>
    struct Tuple<T, T1, tuple::null_type, tuple::null_type, tuple::null_type> {
        Tuple() { }

        Tuple(T a0, T1 a1):
        mA0(a0),
        mA1(a1) {

        }

        bool empty() const { return false; }
        typedef Tuple<T1,
              tuple::null_type,
              tuple::null_type,
              tuple::null_type,
              tuple::null_type> TailType;
        TailType tail() const { return TailType(mA1); }

        typedef Tuple<T, T1, tuple::null_type, tuple::null_type, tuple::null_type> SelfType;

        T get0() const { return this->mA0; }
        T1 get1() const { return this->mA1; }

        template<typename AT, typename AT1>
        Tuple& operator=(const Tuple<AT, AT1, tuple::null_type, tuple::null_type, tuple::null_type>& rhs) {
            this->mA0 = rhs.mA0;
            this->mA1 = rhs.mA1;
            return *this;
        }

        T mA0;
        T1 mA1;
    };

    template<typename T, typename T1, typename T2>
    struct Tuple<T, T1, T2, tuple::null_type, tuple::null_type> {
        Tuple() { }

        Tuple(T a0, T1 a1, T2 a2):
        mA0(a0),
        mA1(a1),
        mA2(a2) {

        }

        bool empty() const { return false; }
        typedef Tuple<T1,
                      T2,
              tuple::null_type,
              tuple::null_type,
              tuple::null_type> TailType;
        TailType tail() const { return TailType(mA1, mA2); }

        typedef Tuple<T, T1, T2, tuple::null_type, tuple::null_type> SelfType;

        T get0() const { return this->mA0; }
        T1 get1() const { return this->mA1; }
        T2 get2() const { return this->mA2; }

        template<typename AT, typename AT1, typename AT2>
        Tuple& operator=(const Tuple<AT, AT1, AT2, tuple::null_type, tuple::null_type>& rhs) {
            this->mA0 = rhs.mA0;
            this->mA1 = rhs.mA1;
            this->mA2 = rhs.mA2;
            return *this;
        }

        T mA0;
        T1 mA1;
        T2 mA2;
    };

    template<typename T, typename T1, typename T2, typename T3>
    struct Tuple<T, T1, T2, T3, tuple::null_type> {
        Tuple() { }

        Tuple(T a0, T1 a1, T2 a2, T3 a3):
        mA0(a0),
        mA1(a1),
        mA2(a2),
        mA3(a3) {

        }

        bool empty() const { return false; }
        typedef Tuple<T1,
                      T2,
                      T3,
              tuple::null_type,
              tuple::null_type> TailType;
        TailType tail() const { return TailType(mA1, mA2, mA3); }

        typedef Tuple<T, T1, T2, T3, tuple::null_type> SelfType;

        T get0() const { return this->mA0; }
        T1 get1() const { return this->mA1; }
        T2 get2() const { return this->mA2; }
        T3 get3() const { return this->mA3; }

        template<typename AT, typename AT1, typename AT2, typename AT3>
        Tuple& operator=(const Tuple<AT, AT1, AT2, AT3, tuple::null_type>& rhs) {
            this->mA0 = rhs.mA0;
            this->mA1 = rhs.mA1;
            this->mA2 = rhs.mA2;
            this->mA3 = rhs.mA3;
            return *this;
        }

        T mA0;
        T1 mA1;
        T2 mA2;
        T3 mA3;
    };

    template<typename T, typename T1, typename T2, typename T3, typename T4>
    struct Tuple {
        Tuple() { }

        Tuple(T a0, T1 a1, T2 a2, T3 a3, T4 a4):
        mA0(a0),
        mA1(a1),
        mA2(a2),
        mA3(a3),
        mA4(a4) {

        }

        bool empty() const { return false; }
        typedef Tuple<T1,
                      T2,
                      T3,
                      T4,
              tuple::null_type> TailType;
        TailType tail() const { return TailType(mA1, mA2, mA3, mA4); }

        typedef Tuple<T, T1, T2, T3, T4> SelfType;

        T get0() const { return this->mA0; }
        T1 get1() const { return this->mA1; }
        T2 get2() const { return this->mA2; }
        T3 get3() const { return this->mA3; }
        T4 get4() const { return this->mA4; }

        template<typename AT, typename AT1, typename AT2, typename AT3, typename AT4>
        Tuple& operator=(const Tuple<AT, AT1, AT2, AT3, AT4>& rhs) {
            this->mA0 = rhs.mA0;
            this->mA1 = rhs.mA1;
            this->mA2 = rhs.mA2;
            this->mA3 = rhs.mA3;
            this->mA4 = rhs.mA4;
            return *this;
        }

        T mA0;
        T1 mA1;
        T2 mA2;
        T3 mA3;
        T4 mA4;
    };

    template<typename T>
    Tuple<T, tuple::null_type, tuple::null_type, tuple::null_type, tuple::null_type>
        MakeTuple(const T& t) {
            return Tuple<T, tuple::null_type, tuple::null_type, tuple::null_type, tuple::null_type>(t);
    }

    template<typename T, typename T1>
    Tuple<T, T1, tuple::null_type, tuple::null_type, tuple::null_type>
        MakeTuple(const T& t, const T1& t1) {
            return Tuple<T, T1, tuple::null_type, tuple::null_type, tuple::null_type>(t, t1);
    }

    template<typename T, typename T1, typename T2>
    Tuple<T, T1, T2, tuple::null_type, tuple::null_type>
        MakeTuple(const T& t, const T1& t1, const T2& t2) {
            return Tuple<T, T1, T2, tuple::null_type, tuple::null_type>(t, t1, t2);
    }

    template<typename T, typename T1, typename T2, typename T3>
    Tuple<T, T1, T2, T3, tuple::null_type>
        MakeTuple(const T& t, const T1& t1, const T2& t2, const T3& t3) {
            return Tuple<T, T1, T2, T3, tuple::null_type>(t, t1, t2, t3);
    }

    template<typename T, typename T1, typename T2, typename T3, typename T4>
    Tuple<T, T1, T2, T3, T4>
        MakeTuple(const T& t, const T1& t1, const T2& t2, const T3& t3, const T4& t4) {
            return Tuple<T, T1, T2, T3, tuple::null_type>(t, t1, t2, t3, t4);
    }

    class FourCC {
    public:
        FourCC();
        FourCC(uint32 f);
        FourCC(const MistString& s);

        bool operator==(const FourCC& rhs) const;
        bool operator!=(const FourCC& rhs) const;
        bool operator<(const FourCC& rhs) const;
        bool operator>(const FourCC& rhs) const;
        bool operator<=(const FourCC& rhs) const;
        bool operator>=(const FourCC& rhs) const;

        bool isValid() const;
        void fromUInt(uint32 f);
        uint32 asUInt() const;

        void fromString(const MistString& s) {
            mist_assert(!s.empty() &&
                       s.size() >= 4);

            this->mFourCC = uint32(s[0] | s[1]<<8 | s[2]<<16 | s[3]<<24);
        }
        MistString toString() const {
            mist_assert(isValid());

            std::wstring str(L"1234");
            str[0] = static_cast<char>((this->mFourCC & 0xFF000000) >> 24);
            str[1] = static_cast<char>((this->mFourCC & 0x00FF0000) >> 16);
            str[2] = static_cast<char>((this->mFourCC & 0x0000FF00) >> 8);
            str[3] = static_cast<char>((this->mFourCC & 0x000000FF));
            return str;
        }

        static MistString ToString(const FourCC& f);
        static FourCC FromString(const MistString& s);

    private:
        uint32 mFourCC;
    };

    inline FourCC::FourCC():
    mFourCC(0) {

    }

    inline FourCC::FourCC(uint32 f):
    mFourCC(f) {

    }

    inline FourCC::FourCC(const MistString& f) {
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

    inline FourCC FourCC::FromString(const MistString& s) {
        FourCC cc;
        cc.fromString(s);
        return cc;
    }

    inline MistString FourCC::ToString(const FourCC& rhs) {
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

        VectorIterator(T& container):
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

    // simple circular buffer impl
    template<typename T>
    class RingBuffer {
    public:
        explicit RingBuffer(size_t size=2):
        mHead(0),
        mTail(0),
        mQueue(size, 0) {
        }

        ~RingBuffer() { }

        void put(const T& data) {
            mQueue[mHead] = data;
            mHead++;
            if(mHead >= mQueue.size()) {
                mHead = 0;
            }
        }

        T get() {
            if(mTail <= mQueue.size()) {
                mTail++;
                return mQueue[mTail-1];
            } else {
                mTail = 1;
                return mQueue[mTail-1];
            }
        }

        void resize(size_t size) {
            mQueue.resize(size);
        }
        size_t getSize() const {
            return mQueue.size();
        }

    private:
        size_t mHead;
        size_t mTail;

        Array<T> mQueue;
    };
    
    template<typename T>
    class ResGuard {
    public:
        static T& GetRes() {
            static T res_instance;
            return res_instance;
        }
    };
    
    template<typename T>
    class AutoListElement {
    public:
        AutoListElement();
        virtual ~AutoListElement();
        
        typedef std::list<T*> ListType;
        static ListType& GetList() {
            return ResGuard<ListType>::GetRes();
        }
        
    public:
        typedef typename ListType::iterator iterator;
        typedef typename ListType::const_iterator const_iterator;
    
    };
    
    template<typename T>
    AutoListElement<T>::AutoListElement() {
        GetList().push_back(static_cast<T*>(this));
    }
    
    template<typename T>
    AutoListElement<T>::~AutoListElement() {
        GetList().remove(static_cast<T*>(this));
    }

} // namespace mist

#endif
