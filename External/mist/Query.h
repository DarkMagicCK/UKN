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
#include <functional>
#include <algorithm>

#include "mist/Interfaces.h"
#include "mist/detail/TypeTraits.h"

namespace mist {
    
    namespace query {
        
        template<typename T>
        struct Query;
        
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
        struct Query: public IEnumerable<typename container_info<_Cont>::element_type > {
            typedef Query<_Cont>                                    self_type;
            typedef typename selector_container_type<_Cont>::type   container_type;
            typedef typename container_info<_Cont>::element_type    element_type;
            typedef element_type                                    value_type;
            typedef typename container_type::iterator               iterator;
            typedef typename container_type::const_iterator         const_iterator;
            
            Query() { }
            
            Query(const _Cont& c):
            mContainer(c) { }
            
            template<typename _Iterator>
            Query(_Iterator begin, _Iterator end):
            mContainer(begin, end) {
                
            }
            
            template<typename _T, size_t _L>
            Query(_T (&arr)[_L]):
            mContainer(arr, arr + _L) {
                
            }
            
            template<typename _OtherCont>
            Query(const _OtherCont& c):
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
            Query<
                std::unordered_map<
                    _R,
                    std::vector<element_type>
                >
            > GroupBy(const std::function<_R(const element_type&)>& pred) const {
                typedef Query<std::unordered_map<_R, std::vector<element_type> > > selector_ret_type;
                
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
            Query<std::vector<_R> > Select(const std::function<_R(const element_type&)>& pred) const {
                typedef Query<std::vector<_R> > result_type;
                
                result_type result;
                typename result_type::container_type& container = result.get();
                for(const_iterator it = mContainer.begin(), end = mContainer.end();
                    it != end;
                    ++it) {
                    container.push_back(pred(*it));
                }
                return result;
            }
            
            // Inner Join
            template<typename _OtherCont, typename _Pred>
            Query<
                std::vector<
                    std::pair<
                        element_type,
                        typename container_info<_OtherCont>::element_type
                    >
                >
            > JoinOn(const _OtherCont& other_cont, _Pred pred) const {
                typedef typename container_info<_OtherCont>::element_type other_element_type;
                typedef typename container_info<_OtherCont>::const_iterator other_const_iterator;
                typedef std::pair<element_type, other_element_type> pair_type;
                typedef std::vector<pair_type> container_type;
                typedef Query<container_type> result_type;
                
                result_type result;
                container_type& container = result.get();
                for(const_iterator it = mContainer.begin(), end = mContainer.end();
                    it != end;
                    ++it) {
                    const element_type& element = *it;
                    for(other_const_iterator other_cont_it = other_cont.begin(), other_end = other_cont.end();
                        other_cont_it != other_end;
                        ++other_cont_it) {
                        const other_element_type& other_element = *other_cont_it;
                    
                        if(pred(element, other_element)) {
                            container.push_back(std::make_pair(element, other_element));
                        }
                    }
                }
                return result;
            }
            
            // Group Join
            template<typename _OtherCont, typename _Pred>
            Query<
                std::vector<
                    std::pair<
                        element_type,
                        std::vector<
                            typename container_info<_OtherCont>::element_type
                        >
                    >
                >
            > JoinOnGroup(const _OtherCont& other_cont, _Pred pred) const {
                typedef typename container_info<_OtherCont>::element_type other_element_type;
                typedef typename container_info<_OtherCont>::const_iterator other_const_iterator;
                typedef std::vector<std::pair<element_type, std::vector<other_element_type> > > container_type;
                typedef Query<container_type> result_type;
                
                result_type result;
                container_type& container = result.get();
                for(const_iterator it = mContainer.begin(), end = mContainer.end();
                    it != end;
                    ++it) {
                    const element_type& element = *it;
                    
                    container.push_back(std::make_pair(element, std::vector<other_element_type>()));
                    typename container_type::reference& container_pos = container.back();
                    for(other_const_iterator other_cont_it = other_cont.begin(), other_end = other_cont.end();
                        other_cont_it != other_end;
                        ++other_cont_it) {
                        const other_element_type& other_element = *other_cont_it;
                        
                        if(pred(element, other_element)) {
                            container_pos.second.push_back(other_element);
                        }
                    }
                }
                return result;
            }
            
            /* Left Outer Join */
            template<typename _OtherCont, typename _Pred, typename _DefaultIfEmpty>
            Query<
                std::vector<
                    std::pair<
                        element_type,
                        typename container_info<_OtherCont>::element_type
                    >
                >
            > JoinOn(const _OtherCont& other_cont, _Pred pred, _DefaultIfEmpty df) const {
                typedef typename container_info<_OtherCont>::element_type other_element_type;
                typedef typename container_info<_OtherCont>::const_iterator other_const_iterator;
                typedef std::pair<element_type, other_element_type> pair_type;
                typedef std::vector<pair_type> container_type;
                typedef Query<container_type> result_type;
                
                result_type result;
                container_type& container = result.get();
                for(const_iterator it = mContainer.begin(), end = mContainer.end();
                    it != end;
                    ++it) {
                    const element_type& element = *it;
                    bool exist = false;
                    for(other_const_iterator other_cont_it = other_cont.begin(), other_end = other_cont.end();
                        other_cont_it != other_end;
                        ++other_cont_it) {
                        const other_element_type& other_element = *other_cont_it;
                        
                        if(pred(element, other_element)) {
                            container.push_back(std::make_pair(element, other_element));
                            exist = true;
                        }
                    }
                    if(!exist) {
                        container.push_back(std::make_pair(element, df()));
                    }
                }
                return result;
            }
            
            // Left Outer Group Join
            template<typename _OtherCont, typename _Pred, typename _DefaultIfEmpty>
            Query<
                std::vector<
                    std::pair<
                        element_type,
                        std::vector<
                            typename container_info<_OtherCont>::element_type
                        >
                    >
                >
            > JoinOnGroup(const _OtherCont& other_cont, _Pred pred, _DefaultIfEmpty df) const {
                typedef typename container_info<_OtherCont>::element_type other_element_type;
                typedef typename container_info<_OtherCont>::const_iterator other_const_iterator;
                typedef std::vector<std::pair<element_type, std::vector<other_element_type> > > container_type;
                typedef Query<container_type> result_type;
                
                result_type result;
                container_type& container = result.get();
                for(const_iterator it = mContainer.begin(), end = mContainer.end();
                    it != end;
                    ++it) {
                    const element_type& element = *it;
                    
                    container.push_back(std::make_pair(element, std::vector<other_element_type>()));
                    typename container_type::reference& container_pos = container.back();
                    for(other_const_iterator other_cont_it = other_cont.begin(), other_end = other_cont.end();
                        other_cont_it != other_end;
                        ++other_cont_it) {
                        const other_element_type& other_element = *other_cont_it;
                        
                        if(pred(element, other_element)) {
                            container_pos.second.push_back(other_element);
                        }
                    }
                    if(container_pos.second.empty()) {
                        container_pos.second.push_back(df());
                    }
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
                Enumerator(const self_type* query):
                mQuery(query),
                mIterator(mQuery->begin()),
                mIndex(0) { }
                
                IEnumerator<element_type>*  clone() const;
                const element_type& 		current() const;
                intPtr                      index() const;
                bool                        next();
                bool                        available() const;
                void                        reset();
                
            private:
                const self_type* mQuery;
                const_iterator mIterator;
                intPtr mIndex;
            };
            
            EnumeratorType* createEnumerator() const { return new Enumerator(this); }
            
        private:
            container_type mContainer;
        };
        
        template<typename _T>
        Query<_T> From(_T container) {
            return Query<_T>(container);
        }
        
        template<typename _T, size_t _LENGTH>
        Query<_T[_LENGTH]> From(_T (&arr)[_LENGTH]) {
            return Query<_T[_LENGTH]>(arr, arr + _LENGTH);
        }
        
        template<typename _Cont>
        IEnumerator<typename Query<_Cont>::element_type >* Query<_Cont>::Enumerator::clone() const {
            return new Query<_Cont>::Enumerator(mQuery);
        }
        
        template<typename _Cont>
        const typename Query<_Cont>::element_type& Query<_Cont>::Enumerator::current() const {
            return const_cast<const Query<_Cont>::element_type&>(*mIterator);
        }
        
        template<typename _Cont>
        intPtr Query<_Cont>::Enumerator::index() const {
            return mIndex;
        }
        
        template<typename _Cont>
        bool Query<_Cont>::Enumerator::next() {
            if(mIndex < mQuery->size()) {
                mIndex ++;
                mIterator ++;
                return true;
            }
            return false;
        }
        
        template<typename _Cont>
        bool Query<_Cont>::Enumerator::available() const {
            return mIndex < mQuery->size();
        }
        
        template<typename _Cont>
        void Query<_Cont>::Enumerator::reset() {
            mIndex = 0;
            mIterator = mQuery->begin();
        }
        
        extern void _TestQuery();
        
    } // namespace query
    
    
}

#endif /* defined(__Project_Unknown__Query__) */
