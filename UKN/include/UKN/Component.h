//
//  Componenet.h
//  Project Unknown
//
//  Created by Robert Bu on 12/10/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Component_h
#define Project_Unknown_Component_h

#include "UKN/Platform.h"
#include "UKN/Uncopyable.h"
#include "UKN/PreDeclare.h"
#include "UKN/Message.h"
#include "UKN/Serializer.h"

#include <map>

namespace ukn {
    
    /**
     * Base Component Class
     **/
    
#define UKN_DEF_COMPONENT_NAME(compo) \
    static ukn_string GetName() { \
        static ukn_string compo_name(#compo); \
        return compo_name; \
    } \
    virtual const ukn_string& getName() const { \
        static ukn_string compo_name(#compo); \
        return compo_name; \
    }
    
    class Component: Uncopyable, public IConfigSerializable {
    public:
        Component();
        // force base class
        virtual ~Component() = 0;
        
        virtual void onSetOwner(ComponentHolder* owner);
      
        virtual void onComponentAdded(Component* compo);
        virtual void onComponentRemoved(Component* compo);
        
        virtual void onMessageReceived(MessageEventArgs& args);
        
        virtual bool serialize(const ConfigParserPtr& config);
        virtual bool deserialize(const ConfigParserPtr& config);
        
        ComponentHolder* getOwner() const;
        
        virtual const ukn_string& getName() const = 0;
        
    private:
        void setOwner(ComponentHolder* owner);
        
        ComponentHolder* mOwner;
    };
    
    class ComponentHolder {
    public:
        ComponentHolder();
        virtual ~ComponentHolder();
        
        void sendMessage(MessageEventArgs& args);
        void sendMessage(const ukn_string& to, MessageEventArgs& args);
        
        void addComponent(ComponentPtr comp);
        ComponentPtr removeComponent(const ukn_string& name);
        
        bool hasComponent(const ukn_string& name);
        Component* getComponent(const ukn_string& name);
        
        virtual void onComponentAdded(Component* compo);
        virtual void onComponentRemoved(Component* compo);
        
    private:
        typedef std::multimap<ukn_string, ComponentPtr> ComponentMap;
        ComponentMap mComponents;
    };
    
} // namespace ukn


#endif
