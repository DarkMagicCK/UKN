//
//  PreDeclare.h
//  Mist
//
//  Created by Robert Bu on 11/22/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTPreDeclare_h
#define MISTPreDeclare_h

#include "mist/Platform.h"
#include "mist/Ptr.h"

#include <string>

namespace mist {
    
    template<int size, typename T>
    struct PointTemplate;
    
    typedef PointTemplate<1, float> float1;
    typedef PointTemplate<2, float> float2;
    typedef PointTemplate<3, float> float3;
    typedef PointTemplate<4, float> float4;
    
    typedef PointTemplate<1, int32> int1;
    typedef PointTemplate<2, int32> int2;
    typedef PointTemplate<3, int32> int3;
    typedef PointTemplate<4, int32> int4;
    
    typedef PointTemplate<1, uint32> uint1;
    typedef PointTemplate<2, uint32> uint2;
    typedef PointTemplate<3, uint32> uint3;
    typedef PointTemplate<4, uint32> uint4;

    class Resource;
    typedef SharedPtr<Resource> ResourcePtr;
    
    class Stream;
    typedef SharedPtr<Stream> StreamPtr;
    
    class ConfigParser;
    typedef SharedPtr<ConfigParser> ConfigParserPtr;
    
    class ResourceFactory;
    typedef SharedPtr<ResourceFactory> ResourceFactoryPtr;
    
    class ResourceLoader;
    typedef SharedPtr<ResourceLoader> ResourceLoaderPtr;
    
    class Entity;
    typedef SharedPtr<Entity> EntityPtr;
    
    class Component;
    typedef SharedPtr<Component> ComponentPtr;
    
    class Task;
    typedef SharedPtr<Task> TaskPtr;
    
    class FSMState;
    typedef SharedPtr<FSMState> FSMStatePtr;
    
    class Condition;
    typedef SharedPtr<Condition> ConditionPtr;
    
    class Operator;
    typedef SharedPtr<Operator> OperatorPtr;
    
    class Action;
    typedef SharedPtr<Action> ActionPtr;
    
    class Trigger;
    typedef SharedPtr<Trigger> TriggerPtr;
    
    class Entity;
    typedef SharedPtr<Entity> EntityPtr;
    
    class Component;
    typedef SharedPtr<Component> ComponentPtr;
    
    class ComponentHolder;
    typedef SharedPtr<ComponentHolder> ComponentHolderPtr;
    
    
} // namespace mist


#endif