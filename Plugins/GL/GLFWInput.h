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
    
    inline int ukn_key_to_glfw(input::Key::KeyCode key) {
		if((key >= input::Key::Num0 && key <= input::Key::Num9) ||
		   (key >= input::Key::A && key <= input::Key::Z))
			return key;
		
		switch(key) {
			case input::Key::F1:           return GLFW_KEY_F1;
			case input::Key::F2:           return GLFW_KEY_F2;
			case input::Key::F3:           return GLFW_KEY_F3;
			case input::Key::F4:           return GLFW_KEY_F4;
			case input::Key::F5:           return GLFW_KEY_F5;
			case input::Key::F6:           return GLFW_KEY_F6;
			case input::Key::F7:           return GLFW_KEY_F7;
			case input::Key::F8:           return GLFW_KEY_F8;
			case input::Key::F9:           return GLFW_KEY_F9;
			case input::Key::F10:          return GLFW_KEY_F10;
			case input::Key::F11:          return GLFW_KEY_F11;
			case input::Key::F12:          return GLFW_KEY_F12;
                
			case input::Key::Escape:       return GLFW_KEY_ESC;
			case input::Key::Backspace:    return GLFW_KEY_BACKSPACE;
			case input::Key::Tab:          return GLFW_KEY_TAB;
			case input::Key::Enter:        return GLFW_KEY_ENTER;
			case input::Key::Space:        return GLFW_KEY_SPACE;
                
			case input::Key::Shift:        return GLFW_KEY_LSHIFT;
			case input::Key::Ctrl:         return GLFW_KEY_LCTRL;
			case input::Key::Alt:          return GLFW_KEY_LALT;
                
			case input::Key::Pause:        return GLFW_KEY_PAUSE;
			case input::Key::CapsLock:     return GLFW_KEY_CAPS_LOCK;
			case input::Key::NumLock:      return GLFW_KEY_KP_NUM_LOCK;
			case input::Key::ScrollLock:   return GLFW_KEY_SCROLL_LOCK;
				
			case input::Key::PageUp:       return GLFW_KEY_PAGEUP;
			case input::Key::PageDown:     return GLFW_KEY_PAGEDOWN;
			case input::Key::Home:         return GLFW_KEY_HOME;
			case input::Key::End:          return GLFW_KEY_END;
			case input::Key::Insert:       return GLFW_KEY_INSERT;
			case input::Key::Delete:       return GLFW_KEY_DEL;
                
			case input::Key::Left:         return GLFW_KEY_LEFT;
			case input::Key::Up:           return GLFW_KEY_UP;
			case input::Key::Right:        return GLFW_KEY_RIGHT;
			case input::Key::Down:         return GLFW_KEY_DOWN;
                
			case input::Key::Numpad0:      return GLFW_KEY_KP_0;
			case input::Key::Numpad1:      return GLFW_KEY_KP_1;
			case input::Key::Numpad2:      return GLFW_KEY_KP_2;
			case input::Key::Numpad3:      return GLFW_KEY_KP_3;
			case input::Key::Numpad4:      return GLFW_KEY_KP_4;
			case input::Key::Numpad5:      return GLFW_KEY_KP_5;
			case input::Key::Numpad6:      return GLFW_KEY_KP_6;
			case input::Key::Numpad7:      return GLFW_KEY_KP_7;
			case input::Key::Numpad8:      return GLFW_KEY_KP_8;
			case input::Key::Numpad9:      return GLFW_KEY_KP_9;
                
			case input::Key::Grave:         return GLFW_KEY_GRAVE_ACCENT;
			case input::Key::Minus:         return GLFW_KEY_MINUS;
			case input::Key::Equals:		return GLFW_KEY_EQUAL;
			case input::Key::Backslash:     return GLFW_KEY_BACKSLASH;
			case input::Key::LeftBracket:	return GLFW_KEY_LEFT_BRACKET;
			case input::Key::RightBracket:	return GLFW_KEY_RIGHT_BRACKET;
			case input::Key::Semicolon:     return GLFW_KEY_SEMICOLON;
			case input::Key::Apostrophe:	return GLFW_KEY_APOSTROPHE;
			case input::Key::Comma:         return GLFW_KEY_COMMA;
			case input::Key::Period:		return GLFW_KEY_PERIOD;
			case input::Key::Slash:         return GLFW_KEY_SLASH;
                
            default:
                return -1;
		}
	}
    
    inline int ukn_mouse_to_glfw(input::Mouse::MouseButton btn) {
        switch(btn) {
            case input::Mouse::LeftButton:
                return GLFW_MOUSE_BUTTON_LEFT;
            case input::Mouse::RightButton:
                return GLFW_MOUSE_BUTTON_RIGHT;
            case input::Mouse::MiddleButton:
                return GLFW_MOUSE_BUTTON_MIDDLE;
            default:
                return -1;
        }
    }
    
    inline input::Mouse::MouseButton glfw_to_ukn_mouse(int key) {
        switch(key) {
            case GLFW_MOUSE_BUTTON_LEFT:
                return input::Mouse::LeftButton;
            case GLFW_MOUSE_BUTTON_RIGHT:
                return input::Mouse::RightButton;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                return input::Mouse::MiddleButton;
                
        }
        return input::Mouse::Null;
    }
	
    inline input::Key::KeyCode glfw_to_ukn_key(int key) {
		if((key >= GLFW_KEY_0 && key <= GLFW_KEY_9) ||
		   (key >= GLFW_KEY_A && key <= GLFW_KEY_Z))
			return (input::Key::KeyCode)key;
		
		switch(key) {
			case GLFW_KEY_F1:           return input::Key::F1;
			case GLFW_KEY_F2:           return input::Key::F2;
			case GLFW_KEY_F3:           return input::Key::F3;
			case GLFW_KEY_F4:           return input::Key::F4;
			case GLFW_KEY_F5:           return input::Key::F5;
			case GLFW_KEY_F6:           return input::Key::F6;
			case GLFW_KEY_F7:           return input::Key::F7;
			case GLFW_KEY_F8:           return input::Key::F8;
			case GLFW_KEY_F9:           return input::Key::F9;
			case GLFW_KEY_F10:          return input::Key::F10;
			case GLFW_KEY_F11:          return input::Key::F11;
			case GLFW_KEY_F12:          return input::Key::F12;
				
			case GLFW_KEY_ESC:          return input::Key::Escape;
			case GLFW_KEY_BACKSPACE:    return input::Key::Backspace;
			case GLFW_KEY_TAB:          return input::Key::Tab;
			case GLFW_KEY_ENTER:        return input::Key::Enter;
			case GLFW_KEY_SPACE:        return input::Key::Space;
				
			case GLFW_KEY_LSHIFT:       return input::Key::Shift;
			case GLFW_KEY_LCTRL:        return input::Key::Ctrl;
			case GLFW_KEY_LALT:         return input::Key::Alt;
				
			case GLFW_KEY_RSHIFT:       return input::Key::Shift;
			case GLFW_KEY_RCTRL:        return input::Key::Ctrl;
			case GLFW_KEY_RALT:         return input::Key::Alt;
				
			case GLFW_KEY_PAUSE:        return input::Key::Pause;
			case GLFW_KEY_CAPS_LOCK:    return input::Key::CapsLock;
			case GLFW_KEY_KP_NUM_LOCK:  return input::Key::NumLock;
			case GLFW_KEY_SCROLL_LOCK:  return input::Key::ScrollLock;
				
			case GLFW_KEY_PAGEUP:       return input::Key::PageUp;
			case GLFW_KEY_PAGEDOWN:     return input::Key::PageDown;
			case GLFW_KEY_HOME:         return input::Key::Home;
			case GLFW_KEY_END:          return input::Key::End;
			case GLFW_KEY_INSERT:       return input::Key::Insert;
			case GLFW_KEY_DEL:          return input::Key::Delete;
				
			case GLFW_KEY_LEFT:         return input::Key::Left;
			case GLFW_KEY_UP:           return input::Key::Up;
			case GLFW_KEY_RIGHT:        return input::Key::Right;
			case GLFW_KEY_DOWN:         return input::Key::Down;
				
			case GLFW_KEY_KP_0:         return input::Key::Numpad0;
			case GLFW_KEY_KP_1:         return input::Key::Numpad1;
			case GLFW_KEY_KP_2:         return input::Key::Numpad2;
			case GLFW_KEY_KP_3:         return input::Key::Numpad3;
			case GLFW_KEY_KP_4:         return input::Key::Numpad4;
			case GLFW_KEY_KP_5:         return input::Key::Numpad5;
			case GLFW_KEY_KP_6:         return input::Key::Numpad6;
			case GLFW_KEY_KP_7:         return input::Key::Numpad7;
			case GLFW_KEY_KP_8:         return input::Key::Numpad8;
			case GLFW_KEY_KP_9:         return input::Key::Numpad9;
				
			case GLFW_KEY_GRAVE_ACCENT:	return input::Key::Grave;
			case GLFW_KEY_MINUS:        return input::Key::Minus;
			case GLFW_KEY_EQUAL:        return input::Key::Equals;
			case GLFW_KEY_BACKSLASH:	return input::Key::Backslash;
			case GLFW_KEY_LEFT_BRACKET: return input::Key::LeftBracket;
			case GLFW_KEY_RIGHT_BRACKET:return input::Key::RightBracket;
			case GLFW_KEY_SEMICOLON:	return input::Key::Semicolon;
			case GLFW_KEY_APOSTROPHE:	return input::Key::Apostrophe;
			case GLFW_KEY_COMMA:        return input::Key::Comma;
			case GLFW_KEY_PERIOD:       return input::Key::Period;
			case GLFW_KEY_SLASH:        return input::Key::Slash;
		}
        
        return input::Key::Null;
	}
    
    inline int ukn_joystick_to_glfw(input::JoyStick::JoyStickButton btn) {
        return (int)btn;
    }
    
    inline input::JoyStick::JoyStickButton glfw_to_ukn_joystick(int key) {
        return (input::JoyStick::JoyStickButton)key;
    }
    
    inline input::Mouse::MouseState glfw_to_ukn_mouse_state(int state) {
        switch(state) {
            case GLFW_PRESS:
                return input::Mouse::Press;
            case GLFW_RELEASE:
                return input::Mouse::Release;
        }
        ukn_assert(0);
    }
    
    inline int ukn_mouse_state_to_glfw(input::Mouse::MouseState state) {
        switch(state) {
            case input::Mouse::Press:
                return GLFW_PRESS;
            case input::Mouse::Release:
                return GLFW_RELEASE;
            default:
                ukn_assert(0);
        }
    }
    
    inline input::Key::KeyState glfw_to_ukn_key_state(int state) {
        switch(state) {
            case GLFW_PRESS:
                return input::Key::Press;
            case GLFW_RELEASE:
                return input::Key::Release;
        }
        ukn_assert(0);
    }
    
    inline int ukn_key_state_to_glfw(input::Key::KeyState state) {
        switch(state) {
            case input::Key::Press:
                return GLFW_PRESS;
            case input::Key::Release:
                return GLFW_RELEASE;
        }
    }
    
    inline input::JoyStick::JoyStickState glfw_to_ukn_joystick_state(int state) {
        switch(state) {
            case GLFW_PRESS:
                return input::JoyStick::Press;
            case GLFW_RELEASE:
                return input::JoyStick::Release;
        }
        ukn_assert(0);
    }
    
    inline int ukn_joystick_state_to_glfw(input::JoyStick::JoyStickState state) {
        switch(state) {
            case input::JoyStick::Press:
                return GLFW_PRESS;
            case input::JoyStick::Release:
                return GLFW_RELEASE;
        }
    }
    
} // namespace ukn

#endif
