//
//  LeapMotion.h
//  Project Unknown
//
//  Created by Robert Bu on 11/16/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_LeapMotion_h
#define Project_Unknown_LeapMotion_h

#include "mist/Platform.h"
#include "mist/Module.h"
#include "mist/ListenerManager.h"

#include "leapmotion/Leap.h"

namespace ukn {
    
    namespace input {
        
        struct LeapMotionListener {
            virtual void onLeapMotionFrame(const Leap::Controller& c) = 0;
        };
        
        class LeapMotionModule:
            public Leap::Listener,
            public mist::Module,
            public mist::ListenerManager<LeapMotionListener> {
        public:
            LeapMotionModule();
            virtual ~LeapMotionModule();
            
        public:
            // mist::Module
            virtual const mist::MistString& getName() const;
            virtual bool init();
            virtual void update();
            virtual void shutdown();
            
        public:
            // Leap::Listener
            virtual void onInit(const Leap::Controller& c);
            virtual void onConnect(const Leap::Controller& c);
            virtual void onDisconnect(const Leap::Controller& c);
            virtual void onFrame(const Leap::Controller& c);
            
        public:
            Leap::Controller* getLeapController() const;
            
        private:
            Leap::Controller* mLeapController;
        };
        
    }
    
}

#endif
