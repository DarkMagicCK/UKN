//
//  Trigger.cpp
//  Project Unknown
//
//  Created by Robert Bu on 12/15/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Trigger.h"
#include "UKN/Util.h"

namespace ukn {
    
    Operator::Operator() {
        
    }
    
    Operator::~Operator() {
        
    }
    
    void Operator::setLeft(const OperatorPtr& left) {
        mLeft = left;
    }
    
    void Operator::setRight(const OperatorPtr& right) {
        mRight = right;
    }
    
    bool Operator::evaluate() const {        
        if(mLeft) {
            if(!mLeft->onEvaluate())
                return false;
        }
        
        if(mRight) {
            if(!mLeft->onEvaluate())
                return false;
        }
        
        if(!mLeft && !mRight)
            return false;
        
        return true;
    }
    
    OperatorAnd::OperatorAnd(const ConditionPtr& cond1, const ConditionPtr& cond2):
    mCond1(cond1),
    mCond2(cond2) {
        
    }
    
    OperatorAnd::~OperatorAnd() {
        
    }
    
    bool OperatorAnd::onEvaluate() const {
        if(mCond1 && mCond2)
            return mCond1->isSatisfied() && mCond2->isSatisfied();
        return false;
    }
    
    OperatorOr::OperatorOr(const ConditionPtr& cond1, const ConditionPtr& cond2):
    mCond1(cond1),
    mCond2(cond2) {
        
    }
    
    OperatorOr::~OperatorOr() {
        
    }
    
    bool OperatorOr::onEvaluate() const {
        if(mCond1 && mCond2)
            return mCond1->isSatisfied() || mCond2->isSatisfied();
        return false;
    }
    
    OperatorNot::OperatorNot(const ConditionPtr& cond):
    mCond(cond) {
        
    }
    
    OperatorNot::~OperatorNot() {
        
    }
    
    bool OperatorNot::onEvaluate() const {
        return !mCond->isSatisfied();
    }
    
    Trigger::Trigger() {
        
    }
    
    Trigger::~Trigger() {
        
    }
    
    void Trigger::execute() {
        if(mCondition) {
            if(mCondition->evaluate()) {
                VectorIterator<ActionList> it(mActionList);
                
                while(it.hasMoreElements()) {
                    ActionPtr act = it.getNext();
                    if(act) {
                        act->execute();
                    }
                }
            }
        }
    }
    
    void Trigger::addAction(const ActionPtr& act) {
        mActionList.push_back(act);
    }
    
    void Trigger::setCondition(const OperatorPtr& cond) {
        mCondition = cond;
    }
    
    const Trigger::ActionList& Trigger::getActionList() const {
        return mActionList;
    }
    
    OperatorPtr& Trigger::getCondition() {
        return mCondition;
    }
    
    const OperatorPtr& Trigger::getCondition() const {
        return mCondition;
    }
    
} // namespace ukn
