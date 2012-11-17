//
//  Cond.h
//  mist
//
//  Created by Robert Bu on 11/12/12.
//  Copyright (c) 2012 Robert Bu. All rights reserved.
//

#ifndef mist_Cond_h
#define mist_Cond_h

#include "../Platform.h"
#include <map>

namespace mist {
    
#ifdef MIST_CPP11
    
    namespace functional {
        
        // scheme-like cond
        
        /*
         (cond
         [expr expr]
         [expr expr]
         [expr expr]
         [else expr]
         )
         */
        template<typename _RT>
        struct Cond {
            typedef std::function<bool()> BooleanExpr;
            typedef std::function<_RT()> Expr;
            
            Cond& If(const BooleanExpr& c, const Expr& e) {
                _exprs.push_back(std::make_pair(c, e));
                return *this;
            }
            
            Cond& Else(const Expr& e) {
                _else = e;
                return *this;
            }
            
            _RT operator()() {
                typedef typename ExprList::iterator ExprIt;
                for(ExprIt it = _exprs.begin(), end = _exprs.end();
                    it != end;
                    ++it) {
                    if((it->first)()) {
                        return (it->second)();
                    }
                }
                
                return _else();
            }
        
            typedef std::vector<std::pair<BooleanExpr, Expr> > ExprList;
            ExprList _exprs;
            Expr _else;
        };
        
#ifdef MIST_VARIADIC_TEMPLATE
        
        template<typename _RT, typename ...ARGS>
        struct CondV {
            typedef std::function<bool(ARGS...)> BooleanExpr;
            typedef std::function<_RT(ARGS...)> Expr;
            
            CondV& If(const BooleanExpr& c, const Expr& e) {
                _exprs.push_back(std::make_pair(c, e));
                return *this;
            }
            
            CondV& Else(const Expr& e) {
                _else = e;
                return *this;
            }
            
            _RT operator()(ARGS... args) {
                typedef typename ExprList::iterator ExprIt;
                for(ExprIt it = _exprs.begin(), end = _exprs.end();
                    it != end;
                    ++it) {
                    if((it->first)(args...)) {
                        return (it->second)(args...);
                    }
                }
                
                if(_else)
                    return _else(args...);
            }
            
            typedef std::vector<std::pair<BooleanExpr, Expr> > ExprList;
            ExprList _exprs;
            Expr _else;
        };
        
#endif
        
    }
        
#endif
}

#endif
