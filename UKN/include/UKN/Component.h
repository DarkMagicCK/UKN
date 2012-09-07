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
    static UknString GetName() { \
        static UknString compo_name(#compo); \
        return compo_name; \
    } \
    virtual const UknString& getName() const { \
        static UknString compo_name(#compo); \
        return compo_name; \
    }
    
    class IComponent: public virtual Interface {
    public:        
        virtual const UknString& getName() const = 0;

        virtual void onComponentAdded(Component* compo) = 0;
        virtual void onComponentRemoved(Component* compo) = 0;
        
        virtual void onReceiveMessage(MessageEventArgs& args) = 0;
    
        virtual ComponentHolder* getOwner() const = 0;
    };
    
    class Component: Uncopyable, public virtual IComponent, public virtual IConfigSerializable {
    public:
        Component();
        // force base class
        virtual ~Component() = 0;
        
        ComponentHolder* getOwner() const;
        
        virtual void onSetOwner(ComponentHolder* owner);
        
        void onComponentAdded(Component* compo);
        void onComponentRemoved(Component* compo);
        
        void onReceiveMessage(MessageEventArgs& args);
        void setOwner(ComponentHolder* owner);
        
    public:
        //
        virtual bool serialize(const ConfigParserPtr& config);
        virtual bool deserialize(const ConfigParserPtr& config);

    private:
        
        ComponentHolder* mOwner;
    };
    
    class ComponentHolder {
    public:
        ComponentHolder();
        virtual ~ComponentHolder();
        
        void notify(Component* c, MessageEventArgs& args);
        void sendMessage(MessageEventArgs& args);
        void sendMessage(const UknString& to, MessageEventArgs& args);
        
        void            addComponent(ComponentPtr comp);
        ComponentPtr    removeComponent(const UknString& name);
        
        bool        hasComponent(const UknString& name);
        Component*  getComponent(const UknString& name);
        
        virtual void onComponentAdded(Component* compo);
        virtual void onComponentRemoved(Component* compo);
        
        // notify component holder
        // from a component
        virtual void onNotified(Component* c, MessageEventArgs& args);
        
    private:
        typedef std::multimap<UknString, ComponentPtr> ComponentMap;
        ComponentMap mComponents;
    };
    
} // namespace ukn


#endif
