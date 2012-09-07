//
//  Window.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Window.h"

namespace ukn {

    Window::Window(const UknString& name):
    mName(name) {
        
    }
    
    Window::~Window() {
        
    }
    
    void Window::setWindowTitle(const UknString& title) {
        mName = title;
        this->setName(title);
    }
    
    const UknString& Window::getWindowTitle() const {
        return mName;
    }
    
    int32 Window::left() const {
        return mLeft;
    }
    
    int32 Window::top() const {
        return mTop;
    }
    
    uint32 Window::width() const {
        return mWidth;
    }
    
    uint32 Window::height() const {
        return mHeight;
    }
    
    
} // namespace ukn