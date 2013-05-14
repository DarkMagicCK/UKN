//
//  Input.h
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Input_h
#define Project_Unknown_Input_h

#include "mist/Event.h"
#include "UKN/PreDeclare.h"

namespace ukn {
    
    namespace input {
        
		   
        enum InputState {
            Press      = 0,
            Release    = 1,
            Move       = 2,
            Wheel      = 3,
		};
        
        // keyboard
        enum KeyCode {
			Nothing = 0,

            Num0 = 0x30,
            Num1,
            Num2,
            Num3,
            Num4,
            Num5,
            Num6,
            Num7,
            Num8,
            Num9,
                
            A = 0x41,
            B,
            C,
            D,
            E,
            F,
            G,
            H,
            I,
            J,
            K,
            L,
            M,
            N,
            O,
            P,
            Q,
            R,
            S,
            T,
            U,
            V,
            W,
            X,
            Y,
            Z,
                
            F1 = 0x70,
            F2,
            F3,
            F4,
            F5,
            F6,
            F7,
            F8,
            F9,
            F10,
            F11,
            F12,
                
            Numpad0 = 0x60,
            Numpad1,
            Numpad2,
            Numpad3,
            Numpad4,
            Numpad5,
            Numpad6,
            Numpad7,
            Numpad8,
            Numpad9,
                
            Escape		= 0x1B,
            Backspace	= 0x08,
            Tab         = 0x09,
            Enter		= 0x0D,
            Space		= 0x20,
                
            Shift	= 0x10,
            Ctrl	= 0x11,
            Alt     = 0x12,
                
            Pause		= 0x13,
            CapsLock	= 0x14,
            NumLock     = 0x90,
            ScrollLock  = 0x91,
                
            PageUp		= 0x21,
            PageDown	= 0x22,
            Home		= 0x24,
            End         = 0x23,
            Insert		= 0x2D,
            Delete		= 0x2E,
                
            Left	= 0x25,
            Up		= 0x26,
            Right	= 0x27,
            Down	= 0x28,
                
            Grave		= 0xC0,
            Minus		= 0xBD,
            Equals		= 0xBB,
            Backslash	= 0xDC,
            LeftBracket	= 0xDB,
            RightBracket= 0xDD,
            Semicolon	= 0xBA,
            Apostrophe  = 0xDE,
            Comma		= 0xBC,
            Period		= 0xBE,
            Slash		= 0xBF,
                
            LeftWin     = 0x5B,
            RightWin    = 0x5C,
            Apps        = 0x5D,
            
            KeyCodeMax  = 256, // reserved
        };
            
        enum KeyFlag {
            FlagShift		= 1U,
            FlagCtrl		= 1U<<1,
            FlagAlt			= 1U<<2,
            FlagCapslock	= 1U<<3,
            FlagScrolllock	= 1U<<4,
            FlagNumlock		= 1U<<5,
            FlagRepeat		= 1U<<6,
        };
        
        // mouse
        enum MouseButton {
            LeftButton = 128,
            RightButton,
            MiddleButton,

            LastButton = LeftButton + 8,
        };
         
        // joystick
        enum JoyStickButton {
            Joystick1 = MouseButton::LastButton + 1,
            Joystick2,
            Joystick3,
            Joystick4,
            Joystick5,
            Joystick6,
            Joystick7,
            Joystick8,
            Joystick9,
            Joystick10,
            Joystick11,
            Joystick12,
            Joystick13,
            Joystick14,
            Joystick15,
            Joystick16,
        };
           
        struct UKN_API KeyEventArgs {
			KeyEventArgs() {

			}

			KeyEventArgs(InputState state, mist::uint16 key, int flag, bool isIME = false):
				state(state),
				key(key),
				flag(flag),
				isIME(isIME) {

			}

            InputState    state;		// state
            mist::uint16  key;			// key code
            int			  flag;
            
            // experimental
            bool    isIME;                  // is ime key
            
            bool isKeyDown(KeyCode k) const;
            bool isKeyUp(KeyCode k) const;
            bool isDigit() const;
            bool isLetter() const;
            bool isPrintable() const;
            bool isShiftFlag() const;
            bool isCtrlFlag() const;
            bool isAltFlag() const;
            
            wchar_t toCharCode() const;
        };
        
        struct UKN_API MouseEventArgs {
			MouseEventArgs() {

			}

			MouseEventArgs(InputState state, mist::uint8 btn, int flag, int x, int y, int wheel):
				state(state),
				button(btn),
				flag(flag),
				x(x),
				y(y),
				wheel(wheel) {
			}

            InputState   state;
            mist::uint8  button;

            int   flag;            
            int   x;
            int   y;
            int   wheel;
            
        };
        
        struct UKN_API JoyStickEventArgs {
            InputState        flag;
            mist::uint8       button;
        };
    }
    
} // namespace ukn

#endif
