//
//  GLFWInput.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/24/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_GLFWInput_h
#define Project_Unknown_GLFWInput_h

#include "glfw/glfw3.h"

#include "UKN/Input.h"

namespace ukn {
    
    inline int ukn_key_to_glfw(input::KeyCode key) {
		if((key >= input::Num0 && key <= input::Num9) ||
		   (key >= input::A && key <= input::Z))
			return key;
		
		switch(key) {
			case input::F1:           return GLFW_KEY_F1;
			case input::F2:           return GLFW_KEY_F2;
			case input::F3:           return GLFW_KEY_F3;
			case input::F4:           return GLFW_KEY_F4;
			case input::F5:           return GLFW_KEY_F5;
			case input::F6:           return GLFW_KEY_F6;
			case input::F7:           return GLFW_KEY_F7;
			case input::F8:           return GLFW_KEY_F8;
			case input::F9:           return GLFW_KEY_F9;
			case input::F10:          return GLFW_KEY_F10;
			case input::F11:          return GLFW_KEY_F11;
			case input::F12:          return GLFW_KEY_F12;
                
			case input::Escape:       return GLFW_KEY_ESC;
			case input::Backspace:    return GLFW_KEY_BACKSPACE;
			case input::Tab:          return GLFW_KEY_TAB;
			case input::Enter:        return GLFW_KEY_ENTER;
			case input::Space:        return GLFW_KEY_SPACE;
                
			case input::Shift:        return GLFW_KEY_LSHIFT;
			case input::Ctrl:         return GLFW_KEY_LCTRL;
			case input::Alt:          return GLFW_KEY_LALT;
                
			case input::Pause:        return GLFW_KEY_PAUSE;
			case input::CapsLock:     return GLFW_KEY_CAPS_LOCK;
			case input::NumLock:      return GLFW_KEY_KP_NUM_LOCK;
			case input::ScrollLock:   return GLFW_KEY_SCROLL_LOCK;
				
			case input::PageUp:       return GLFW_KEY_PAGEUP;
			case input::PageDown:     return GLFW_KEY_PAGEDOWN;
			case input::Home:         return GLFW_KEY_HOME;
			case input::End:          return GLFW_KEY_END;
			case input::Insert:       return GLFW_KEY_INSERT;
			case input::Delete:       return GLFW_KEY_DEL;
                
			case input::Left:         return GLFW_KEY_LEFT;
			case input::Up:           return GLFW_KEY_UP;
			case input::Right:        return GLFW_KEY_RIGHT;
			case input::Down:         return GLFW_KEY_DOWN;
                
			case input::Numpad0:      return GLFW_KEY_KP_0;
			case input::Numpad1:      return GLFW_KEY_KP_1;
			case input::Numpad2:      return GLFW_KEY_KP_2;
			case input::Numpad3:      return GLFW_KEY_KP_3;
			case input::Numpad4:      return GLFW_KEY_KP_4;
			case input::Numpad5:      return GLFW_KEY_KP_5;
			case input::Numpad6:      return GLFW_KEY_KP_6;
			case input::Numpad7:      return GLFW_KEY_KP_7;
			case input::Numpad8:      return GLFW_KEY_KP_8;
			case input::Numpad9:      return GLFW_KEY_KP_9;
                
			case input::Grave:         return GLFW_KEY_GRAVE_ACCENT;
			case input::Minus:         return GLFW_KEY_MINUS;
			case input::Equals:		return GLFW_KEY_EQUAL;
			case input::Backslash:     return GLFW_KEY_BACKSLASH;
			case input::LeftBracket:	return GLFW_KEY_LEFT_BRACKET;
			case input::RightBracket:	return GLFW_KEY_RIGHT_BRACKET;
			case input::Semicolon:     return GLFW_KEY_SEMICOLON;
			case input::Apostrophe:	return GLFW_KEY_APOSTROPHE;
			case input::Comma:         return GLFW_KEY_COMMA;
			case input::Period:		return GLFW_KEY_PERIOD;
			case input::Slash:         return GLFW_KEY_SLASH;
                
            default:
                return -1;
		}
	}
    
    inline int ukn_mouse_to_glfw(input::MouseButton btn) {
        switch(btn) {
            case input::LeftButton:
                return GLFW_MOUSE_BUTTON_LEFT;
            case input::RightButton:
                return GLFW_MOUSE_BUTTON_RIGHT;
            case input::MiddleButton:
                return GLFW_MOUSE_BUTTON_MIDDLE;
            default:
                return -1;
        }
    }
    
    inline uint8 glfw_to_ukn_mouse(int key) {
        switch(key) {
            case GLFW_MOUSE_BUTTON_LEFT:
                return input::LeftButton;
            case GLFW_MOUSE_BUTTON_RIGHT:
                return input::RightButton;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                return input::MiddleButton;
                
        }
        return input::Nothing;
    }
	
    inline input::KeyCode glfw_to_ukn_key(int key) {
		if((key >= GLFW_KEY_0 && key <= GLFW_KEY_9) ||
		   (key >= GLFW_KEY_A && key <= GLFW_KEY_Z))
			return (input::KeyCode)key;
		
		switch(key) {
			case GLFW_KEY_F1:           return input::F1;
			case GLFW_KEY_F2:           return input::F2;
			case GLFW_KEY_F3:           return input::F3;
			case GLFW_KEY_F4:           return input::F4;
			case GLFW_KEY_F5:           return input::F5;
			case GLFW_KEY_F6:           return input::F6;
			case GLFW_KEY_F7:           return input::F7;
			case GLFW_KEY_F8:           return input::F8;
			case GLFW_KEY_F9:           return input::F9;
			case GLFW_KEY_F10:          return input::F10;
			case GLFW_KEY_F11:          return input::F11;
			case GLFW_KEY_F12:          return input::F12;
				
			case GLFW_KEY_ESC:          return input::Escape;
			case GLFW_KEY_BACKSPACE:    return input::Backspace;
			case GLFW_KEY_TAB:          return input::Tab;
			case GLFW_KEY_ENTER:        return input::Enter;
			case GLFW_KEY_SPACE:        return input::Space;
				
			case GLFW_KEY_LSHIFT:       return input::Shift;
			case GLFW_KEY_LCTRL:        return input::Ctrl;
			case GLFW_KEY_LALT:         return input::Alt;
				
			case GLFW_KEY_RSHIFT:       return input::Shift;
			case GLFW_KEY_RCTRL:        return input::Ctrl;
			case GLFW_KEY_RALT:         return input::Alt;
				
			case GLFW_KEY_PAUSE:        return input::Pause;
			case GLFW_KEY_CAPS_LOCK:    return input::CapsLock;
			case GLFW_KEY_KP_NUM_LOCK:  return input::NumLock;
			case GLFW_KEY_SCROLL_LOCK:  return input::ScrollLock;
				
			case GLFW_KEY_PAGEUP:       return input::PageUp;
			case GLFW_KEY_PAGEDOWN:     return input::PageDown;
			case GLFW_KEY_HOME:         return input::Home;
			case GLFW_KEY_END:          return input::End;
			case GLFW_KEY_INSERT:       return input::Insert;
			case GLFW_KEY_DEL:          return input::Delete;
				
			case GLFW_KEY_LEFT:         return input::Left;
			case GLFW_KEY_UP:           return input::Up;
			case GLFW_KEY_RIGHT:        return input::Right;
			case GLFW_KEY_DOWN:         return input::Down;
				
			case GLFW_KEY_KP_0:         return input::Numpad0;
			case GLFW_KEY_KP_1:         return input::Numpad1;
			case GLFW_KEY_KP_2:         return input::Numpad2;
			case GLFW_KEY_KP_3:         return input::Numpad3;
			case GLFW_KEY_KP_4:         return input::Numpad4;
			case GLFW_KEY_KP_5:         return input::Numpad5;
			case GLFW_KEY_KP_6:         return input::Numpad6;
			case GLFW_KEY_KP_7:         return input::Numpad7;
			case GLFW_KEY_KP_8:         return input::Numpad8;
			case GLFW_KEY_KP_9:         return input::Numpad9;
				
			case GLFW_KEY_GRAVE_ACCENT:	return input::Grave;
			case GLFW_KEY_MINUS:        return input::Minus;
			case GLFW_KEY_EQUAL:        return input::Equals;
			case GLFW_KEY_BACKSLASH:	return input::Backslash;
			case GLFW_KEY_LEFT_BRACKET: return input::LeftBracket;
			case GLFW_KEY_RIGHT_BRACKET:return input::RightBracket;
			case GLFW_KEY_SEMICOLON:	return input::Semicolon;
			case GLFW_KEY_APOSTROPHE:	return input::Apostrophe;
			case GLFW_KEY_COMMA:        return input::Comma;
			case GLFW_KEY_PERIOD:       return input::Period;
			case GLFW_KEY_SLASH:        return input::Slash;
		}
        
        return input::Nothing;
	}
    
    inline int ukn_joystick_to_glfw(input::JoyStickButton btn) {
        return (int)btn;
    }
    
    inline input::JoyStickButton glfw_to_ukn_joystick(int key) {
        return (input::JoyStickButton)key;
    }
    
    inline input::InputState glfw_to_ukn_mouse_state(int state) {
        switch(state) {
            case GLFW_PRESS:
                return input::Press;
            case GLFW_RELEASE:
                return input::Release;
        }
        mist_assert(0);
    }
    
    inline int ukn_mouse_state_to_glfw(input::InputState state) {
        switch(state) {
            case input::Press:
                return GLFW_PRESS;
            case input::Release:
                return GLFW_RELEASE;
            default:
                mist_assert(0);
        }
    }
    
    inline input::InputState glfw_to_ukn_key_state(int state) {
        switch(state) {
            case GLFW_PRESS:
                return input::Press;
            case GLFW_RELEASE:
                return input::Release;
        }
        mist_assert(0);
    }
    
    inline int ukn_key_state_to_glfw(input::InputState state) {
        switch(state) {
            case input::Press:
                return GLFW_PRESS;
            case input::Release:
                return GLFW_RELEASE;
            default:
                return 0;
        }
        return 0;
    }
    
    inline input::InputState glfw_to_ukn_joystick_state(int state) {
        switch(state) {
            case GLFW_PRESS:
                return input::InputState::Press;
            case GLFW_RELEASE:
                return input::InputState::Release;
        }
        mist_assert(0);
        
    }
    
    inline int ukn_joystick_state_to_glfw(input::InputState state) {
        switch(state) {
            case input::Press:
                return GLFW_PRESS;
            case input::Release:
                return GLFW_RELEASE;
            default:
                return 0;
        }
        mist_assert(0);
    }
    
} // namespace ukn

#endif
