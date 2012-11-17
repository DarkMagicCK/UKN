//
//  Componenet.h
//  Mist
//
//  Created by Robert Bu on 12/10/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTComponent_h
#define MISTComponent_h

#include "mist/Platform.h"
#include "mist/Uncopyable.h"
#include "mist/PreDeclare.h"
#include "mist/Message.h"
#include "mist/Serializer.h"

#include <map>

namespace mist {
    
    /**
     * Base Component Class
     **/
    
#define UKN_DEF_COMPONENT_NAME(compo) \
    static MistString GetName() { \
        static MistString compo_name(#compo); \
        return compo_name; \
    } \
    virtual const MistString& getName() const { \
        static MistString compo_name(#compo); \
        return compo_name; \
    }
    
    class IComponent: public virtual Interface {
    public:        
        virtual const MistString& getName() const = 0;

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
        void sendMessage(const MistString& to, MessageEventArgs& args);
        
        void            addComponent(ComponentPtr comp);
        ComponentPtr    removeComponent(const MistString& name);
        
        bool        hasComponent(const MistString& name);
        Component*  getComponent(const MistString& name);
        
        virtual void onComponentAdded(Component* compo);
        virtual void onComponentRemoved(Component* compo);
        
        // notify component holder
        // from a component
        virtual void onNotified(Component* c, MessageEventArgs& args);
        
    private:
        typedef std::multimap<MistString, ComponentPtr> ComponentMap;
        ComponentMap mComponents;
    };
    
} // namespace mist


#endif
