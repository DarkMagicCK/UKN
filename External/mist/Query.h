//
//  Query.h
//  Project Unknown
//
//  Created by Robert Bu on 12/10/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef __Project_Unknown__Query__
#define __Project_Unknown__Query__

#include <vector>
#include <map>
#include <unordered_map>

#include "mist/Interfaces.h"
#include "mist/detail/TypeTraits.h"

namespace mist {
    
    namespace query {
        
        template<typename T>
        struct container_info {
            typedef typename T::iterator iterator;
            typedef typename T::const_iterator const_iterator;
            typedef typename T::value_type element_type;
        };
        
        template<typename T, size_t LENGTH>
        struct container_info<T[LENGTH]> {
            typedef T*          iterator;
            typedef const T*    const_iterator;
            typedef T           element_type;
        };
        
        template<typename T>
        struct selector_container_type {
            typedef T type;
        };
        
        template<typename T, size_t LENGTH>
        struct selector_container_type<T[LENGTH]> {
            typedef std::vector<T> type;
        };
        
        enum OrderType {
            Ascending,
            Descending,
        };
        
        template<typename _Cont>
        struct Selector: public IEnumerable<typename container_info<_Cont>::element_type > {
            typedef Selector<_Cont>                                 self_type;
            typedef typename selector_container_type<_Cont>::type   container_type;
            typedef typename container_info<_Cont>::element_type    element_type;
            typedef typename container_type::iterator               iterator;
            typedef typename container_type::const_iterator         const_iterator;
            
            Selector() { }
            
            Selector(const _Cont& c):
            mContainer(c) { }
            
            template<typename _Iterator>
            Selector(_Iterator begin, _Iterator end):
            mContainer(begin, end) {
                
            }
            
            template<typename _T, size_t _L>
            Selector(_T (&arr)[_L]):
            mContainer(arr, arr + _L) {
                
            }
            
            template<typename _OtherCont>
            Selector(const _OtherCont& c):
            mContainer(c.begin(), c.end()) {
                
            }
        
            template<typename _Pred>
            self_type& Where(_Pred pred) {
                mContainer.erase(mContainer.begin(),
                                 std::remove_if(mContainer.begin(), mContainer.end(), pred));
                return *this;
            }
            
            template<typename _Pred>
            self_type& OrderBy(_Pred pred, OrderType order = Ascending) {
                std::sort(mContainer.begin(),
                          mContainer.end(),
                          [&](const element_type& v1, const element_type& v2) {
                              return (order == Ascending) ? (pred(v1) < pred(v2)) : (pred(v1) > pred(v2));
                          });
                return *this;
            }
            
            self_type& OrderBy(OrderType order = Ascending) {
                return this->OrderBy([](element_type t) { return t; },
                                     order);
            }
            
            template<typename _R>
            Selector<
                std::unordered_map<
                    _R,
                    std::vector<element_type>
                >
            > GroupBy(const std::function<_R(const element_type&)>& pred) const {
                typedef Selector<std::unordered_map<_R, std::vector<element_type> > > selector_ret_type;
                
                selector_ret_type result;
                typename selector_ret_type::container_type& container = result.get();
                for(const_iterator it = mContainer.begin(), end = mContainer.end();
                    it != end;
                    ++it) {
                    container[pred(*it)].push_back(*it);
                }
                return result;
            }
            
            template<typename _R>
            Selector<std::vector<_R> > Select(const std::function<_R(const element_type&)>& pred) const {
                typedef Selector<std::vector<_R> > selector_ret_type;
                
                selector_ret_type result;
                typename selector_ret_type::container_type& container = result.get();
                for(const_iterator it = mContainer.begin(), end = mContainer.end();
                    it != end;
                    ++it) {
                    container.push_back(pred(*it));
                }
                return result;
            }
            
            operator container_type() {
                return mContainer;
            }
            
            container_type& get() {
                return mContainer;
            }
            
            const container_type& get() const {
                return mContainer;
            }
            
            size_t size() const { return mContainer.size(); }
            
            element_type& operator[](size_t index) { return mContainer[index]; }
            const element_type& operator[](size_t index) const { return mContainer[index]; }
            
            iterator begin() { return mContainer.begin(); }
            const_iterator begin() const { return mContainer.begin(); }
            
            iterator end() { return mContainer.end(); }
            const_iterator end() const { return mContainer.end(); }
            
            
            typedef IEnumerator<element_type> EnumeratorType;
            struct Enumerator: public IEnumerator<element_type> {
                Enumerator(const self_type* selector):
                mSelector(selector),
                mIterator(mSelector->begin()),
                mIndex(0) { }
                
                IEnumerator<element_type>*  clone() const;
                const element_type& 		current() const;
                intPtr                      index() const;
                bool                        next();
                bool                        available() const;
                void                        reset();
                
            private:
                const self_type* mSelector;
                const_iterator mIterator;
                intPtr mIndex;
            };
            
            EnumeratorType* createEnumerator() const { return new Enumerator(this); }
            
        private:
            container_type mContainer;
        };
        
        template<typename _T>
        Selector<_T> From(_T container) {
            return Selector<_T>(container);
        }
        
        template<typename _T, size_t _LENGTH>
        Selector<_T[_LENGTH]> From(_T (&arr)[_LENGTH]) {
            return Selector<_T[_LENGTH]>(arr, arr + _LENGTH);
        }
        
        template<typename _Cont>
        IEnumerator<typename Selector<_Cont>::element_type >* Selector<_Cont>::Enumerator::clone() const {
            return new Selector<_Cont>::Enumerator(mSelector);
        }
        
        template<typename _Cont>
        const typename Selector<_Cont>::element_type& Selector<_Cont>::Enumerator::current() const {
            return const_cast<const Selector<_Cont>::element_type&>(*mIterator);
        }
        
        template<typename _Cont>
        intPtr Selector<_Cont>::Enumerator::index() const {
            return mIndex;
        }
        
        template<typename _Cont>
        bool Selector<_Cont>::Enumerator::next() {
            if(mIndex < mSelector->size()) {
                mIndex ++;
                mIterator ++;
                return true;
            }
            return false;
        }
        
        template<typename _Cont>
        bool Selector<_Cont>::Enumerator::available() const {
            return mIndex < mSelector->size();
        }
        
        template<typename _Cont>
        void Selector<_Cont>::Enumerator::reset() {
            mIndex = 0;
            mIterator = mSelector->begin();
        }
        
        extern void _TestQuery();
        
    } // namespace query
    
    
}

#endif /* defined(__Project_Unknown__Query__) */
