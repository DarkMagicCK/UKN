//
//  Window.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Window.h"

namespace ukn {

    Window::GlobalUpdateEvent Window::mGlobalUpdate;
    
    Window::Window(const ukn_string& name):
    mName(name) {
        
    }
    
    Window::~Window() {
        
    }
    
    
} // namespace ukn