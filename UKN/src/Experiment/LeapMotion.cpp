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
        
        LeapMotionModule::LeapMotionModule():
        mLeapController(0) {
            
        }
        
        LeapMotionModule::~LeapMotionModule() {
            if(mLeapController) {
                delete mLeapController;
            }
        }
        
        const mist::MistString& LeapMotionModule::getName() const {
            static mist::MistString name = L"LeapMotion";
            return name;
        }
        
        bool LeapMotionModule::init() {
            mLeapController = new Leap::Controller(this);
            return mLeapController != 0;
        }
        
        void LeapMotionModule::update() {
            // pass
        }
        
        void LeapMotionModule::shutdown() {
            if(mLeapController) {
                delete mLeapController;
            }
        }
        
        void LeapMotionModule::onInit(const Leap::Controller& c) {
            mist::log_info(L"LeapMotion initialized");
        }
        
        void LeapMotionModule::onConnect(const Leap::Controller& c) {
            mist::log_info(L"LeapMotion connected");
        }
        
        void LeapMotionModule::onDisconnect(const Leap::Controller& c) {
            mist::log_info(L"LeapMotion disconnected");
        }
        
        void LeapMotionModule::onFrame(const Leap::Controller& c) {
            this->publishListenerEvent<const Leap::Controller&>(&LeapMotionListener::onLeapMotionFrame, c);
        }
        
        Leap::Controller* LeapMotionModule::getLeapController() const {
            return mLeapController;
        }
        
    }
    
}