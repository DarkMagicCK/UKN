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

namespace mist {
    
    namespace query {
        
        template<typename T>
        struct container_info {
            typedef typename T::iterator iterator;
            typedef typename T::const_iterator const_iterator;
            typedef typename T::reference element_type;
        };
        
        template<typename T, size_t LENGTH>
        struct container_info<T[LENGTH]> {
            typedef T*          iterator;
            typedef const T*    const_iterator;
            typedef T           element_type;
            typedef T*          pointer_type;
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
        struct Selector {
            typedef Selector<_Cont>                                 self_type;
            typedef typename selector_container_type<_Cont>::type   container_type;
            typedef typename container_info<_Cont>::element_type    element_type;
            typedef typename container_type::iterator               iterator;
            typedef typename container_type::const_iterator         const_iterator;
            
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
        
            template<typename Pred>
            self_type Where(const Pred& pred) const {
                container_type result;
                for(const_iterator it = mContainer.begin(), end = mContainer.end();
                    it != end;
                    ++it) {
                    if(pred(*it)) {
                        result.push_back(*it);
                    }
                }
                return self_type(result);
            }
            
            template<typename Pred>
            self_type Orderby(const Pred& pred, OrderType order = Ascending) const {
                container_type result = this->select();
                std::sort(result.begin(),
                          result.end(),
                          [&](const element_type& v1, const element_type& v2) {
                              return (order == Ascending) ? (pred(v1) < pred(v2)) : (pred(v1) > pred(v2));
                          });
                return self_type(result);
            }
            
            self_type Orderby(OrderType order = Ascending) const {
                return this->Orderby([](element_type t) { return t; },
                                     order);
            }
            
            operator container_type() const {
                return mContainer;
            }
            
            const container_type select() const {
                return mContainer;
            }
            
            size_t size() const { return mContainer.size(); }
            
            element_type& operator[](size_t index) { return mContainer[index]; }
            const element_type& operator[](size_t index) const { return mContainer[index]; }
            
            iterator begin() { return mContainer.begin(); }
            const_iterator begin() const { return mContainer.begin(); }
            
            iterator end() { return mContainer.end(); }
            const_iterator end() const { return mContainer.end(); }
            
        private:
            container_type mContainer;
        };
        
        template<typename _T>
        struct _Selector {
            static Selector<_T> Select(const _T& container) {
                return Selector<_T>(container);
            }
        };
        
        template<typename _T, size_t _L>
        struct _Selector<_T[_L]> {
            static Selector<_T[_L]> Select(_T arr[_L]) {
                return Selector<_T[_L]>(arr, arr + _L);
            }
        };
        
        template<typename _T>
        Selector<_T> Select(_T container) {
            return Selector<_T>(container);
        }
        
        template<typename _T, size_t _LENGTH>
        Selector<_T[_LENGTH]> Select(_T (&arr)[_LENGTH]) {
            return Selector<_T[_LENGTH]>(arr, arr + _LENGTH);
        }
        
        extern void _TestQuery();
        
    } // namespace query
    
    
}

#endif /* defined(__Project_Unknown__Query__) */
