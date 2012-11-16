//
//  LeapMotion.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/16/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "ukn/LeapMotion.h"

#include "mist/Logger.h"

namespace ukn {
    
    namespace input {
        
        LeapMotionListener::LeapMotionListener():
        mLeapController(0) {
            
        }
        
        LeapMotionListener::~LeapMotionListener() {
            
        }
        
        const mist::MistString& LeapMotionListener::getName() const {
            static mist::MistString name = L"LeapMotion";
            return name;
        }
        
        bool LeapMotionListener::init() {
            mLeapController = new Leap::Controller(this);
            return mLeapController != 0;
        }
        
        void LeapMotionListener::update() {
            // pass
        }
        
        void LeapMotionListener::shutdown() {
            if(mLeapController) {
                delete mLeapController;
            }
        }
        
        void LeapMotionListener::onInit(const Leap::Controller& c) {
            mist::log_info("LeapMotion initialized");
        }
        
        void LeapMotionListener::onConnect(const Leap::Controller& c) {
            mist::log_info("LeapMotion connected");
        }
        
        void LeapMotionListener::onDisconnect(const Leap::Controller& c) {
            mist::log_info("LeapMotion disconnected");
        }
        
        void LeapMotionListener::onFrame(const Leap::Controller& c) {
            
        }
        
        Leap::Controller* LeapMotionListener::getLeapController() const {
            return mLeapController;
        }
        
    }
    
}