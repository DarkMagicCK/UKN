//
//  Player.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/28/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_DF_Player_h
#define Project_DF_Player_h

#include "../Character.h"
#include "../AnimationLayer.h"
#include "../Movable.h"

namespace df {
    
    class Player: public Character, public virtual ukn::IConfigSerializable {
    public:
        Player();
        virtual ~Player();
        
        // components
    public:
        virtual void onComponentAdded(ukn::Component* component);
        virtual void onComponentRemoved(ukn::Component* component);
        
        // character
    public:
        virtual void onRender();
        virtual void onUpdate();
        
        // config serialization
    public:
        virtual bool serialize(const ukn::ConfigParserPtr& config);
        virtual bool deserialize(const ukn::ConfigParserPtr& config);
        
    public:        
        void            setPosition(const ukn::Vector2& pos);
        ukn::Vector2    getPosition() const;
        void            moveTo(const ukn::Vector2& pos);
        
    public:
        Movable mMovable;
        float   mSpeed;
        
        SharedPtr<AnimationPlayer> mRenderer;
    };
    
}


#endif
