//
//  Trigger.h
//  Project Unknown
//
//  Created by Robert Bu on 12/10/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Trigger_h
#define Project_Unknown_Trigger_h

#include "UKN/Platform.h"
#include "UKN/Ptr.h"
#include "UKN/PreDeclare.h"
#include "UKN/Event.h"

namespace ukn {
    
    class Condition;
    typedef SharedPtr<Condition> ConditionPtr;
    
    class Operator;
    typedef SharedPtr<Operator> OperatorPtr;
    
    class Action;
    typedef SharedPtr<Action> Actionptr;

    class Condition {
    public:
        virtual ~Condition() { }
        
        virtual bool isSatisfied() = 0;
    };
    
    class Operator {
    public:
        Operator();
        virtual ~Operator();
        
        void setLeft(const OperatorPtr& left);
        void setRight(const OperatorPtr& right);
        
        bool evaluate() const;
        
        virtual bool onEvaluate() const = 0;
        
    private:
        OperatorPtr mLeft;
        OperatorPtr mRight;
    };
    
    class OperatorAnd: public Operator {
    public:
        OperatorAnd(const ConditionPtr& cond1, const ConditionPtr& cond2);
        ~OperatorAnd();
        
        bool onEvaluate() const;
        
    private:
        ConditionPtr mCond1;
        ConditionPtr mCond2;
    };
    
    class OperatorOr: public Operator {
    public:
        OperatorOr(const ConditionPtr& cond1, const ConditionPtr& cond2);
        ~OperatorOr();
        
        bool onEvaluate() const;
        
    private:
        ConditionPtr mCond1;
        ConditionPtr mCond2;
    };
    
    class OperatorNot: public Operator {
    public:
        OperatorNot(const ConditionPtr& cond1);
        ~OperatorNot();
        
        bool onEvaluate() const;
        
    private:
        ConditionPtr mCond;
    };
    
    class Action {
    public:
        virtual ~Action() { }
        
        virtual void execute() = 0;
    };
    
    class Trigger {
    public:
        Trigger();
        ~Trigger();
    
        void execute();
        
        void addAction(const ActionPtr& act);
        void setCondition(const OperatorPtr& cond);
        
        typedef std::list<ActionPtr> ActionList;
        const ActionList& getActionList() const;
        
        OperatorPtr& getCondition();
        const OperatorPtr& getCondition() const;
        
    private:
        ActionList mActionList;
        
        OperatorPtr mCondition;
    };
    
    
} // namespace ukn

#endif
