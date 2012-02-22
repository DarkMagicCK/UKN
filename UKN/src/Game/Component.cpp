//
//  Component.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/9/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "UKN/Component.h"
#include "UKN/Entity.h"
#include "UKN/Preprocessor.h"

namespace ukn {
    
    Component::Component():
    mOwner(0) {
        
    }
    
    Component::~Component() {
        
    }
    
    void Component::onSetOwner(ComponentHolder* owner) {
        
    }
    
    ComponentHolder* Component::getOwner() const {
        return mOwner;
    }
    
    void Component::setOwner(ComponentHolder* owner) {
        onSetOwner(owner);
        
        mOwner = owner;
    }
    
    bool Component::serialize(const ConfigParserPtr& config) {
        return false;
    }
    
    bool Component::deserialize(const ConfigParserPtr& config) {
        return false;
    }
    
    void Component::onComponentAdded(Component* compo) {
        
    }
    
    void Component::onComponentRemoved(Component* compo) {
        
    }
    
    void Component::onReceiveMessage(MessageEventArgs& args) {
        
    }
    
    ComponentHolder::ComponentHolder() {
        
    }
    
    ComponentHolder::~ComponentHolder() {
        
    }
    
    void ComponentHolder::sendMessage(MessageEventArgs& args) {
        ComponentMap::iterator it = mComponents.begin();
        for(; it != mComponents.end(); ++it)
            it->second->onReceiveMessage(args);
    }
    
    void ComponentHolder::sendMessage(const ukn_string& to, MessageEventArgs& args) {
        ComponentMap::iterator it = mComponents.begin();
        for(; it != mComponents.end(); ++it)
            if(it->second->getName() == to)
                it->second->onReceiveMessage(args);
    }
    
    void ComponentHolder::addComponent(ComponentPtr comp) {
        mComponents.insert(std::make_pair(comp->getName(), comp));
    }
    
    ComponentPtr ComponentHolder::removeComponent(const ukn_string& name) {
        ComponentMap::iterator it = mComponents.find(name);
        if(it != mComponents.end()) {
            ComponentPtr comp = it->second;
            mComponents.erase(it);
            return comp;
        }
        return ComponentPtr();
    }
    
    bool ComponentHolder::hasComponent(const ukn_string& name) {
        ComponentMap::iterator it = mComponents.find(name);
        return it != mComponents.end();
    }
    
    Component* ComponentHolder::getComponent(const ukn_string& name) {
        ComponentMap::iterator it = mComponents.find(name);
        return it != mComponents.end() ? it->second.get() : 0;
    }
    
    void ComponentHolder::onComponentAdded(Component* compo) {
        // 
    }
    
    void ComponentHolder::onComponentRemoved(Component* compo) {
        //
    }
    
    void ComponentHolder::notify(Component* c, MessageEventArgs& args){
        this->onNotified(c, args);
    }
    
    void ComponentHolder::onNotified(Component* c, MessageEventArgs& args) {
        //
    }
    
    
} // namespace ukn
