//
//  Input.h
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Input_h
#define Project_Unknown_Input_h

#include "UKN/Key.h"

namespace ukn {
    
    struct InputEvent {
        int		type;			// event type
		int		key;			// key code
		int		flags;			// event flags
		int		chr;			// character code
		int		wheel;			// wheel shift
		float	x;				// mouse cursor x-coordinate
		float	y;				// mouse cursor y-coordinate
        bool    isIME;          // is ime key
        
        bool isKeyDown(int k) const {
            return (k == this->key && type == key::KeyDown);
        }
        
        bool isKeyUp(int k) const {
            return (k == this->key && type == key::keyUp);
        }
        
        bool isDigit() const {
            return this->key >= 48 && this->key <= 57;
        }
        
        bool isLetter() const {
            return (((this->key >= 65 && this->key <= 90)
                     || (this->key >= 97 && this->key <= 122)
                     || (this->key >= 192 && this->key <= 255))
                    && (this->key != 215) && (this->key != 247));
        }
        
        bool isPrintable() const {
            if((this->key >= key::Num0 && key <= key::Num9) ||
               (this->key >= key::A && key <= key::Z) ||
               (this->key == key::Grave) ||
               (this->key == key::Minus) ||
               (this->key == key::Equals) ||
               (this->key == key::Backslash) ||
               (this->key == key::LeftBracket) ||
               (this->key == key::RightBracket) ||
               (this->key == key::Semicolon) ||
               (this->key == key::Apostrophe) ||
               (this->key == key::Comma) ||
               (this->key == key::Period) ||
               (this->key == key::Slash) ||
               (this->key == key::Space)
               )
                return true;
            return false;
        }
        
        bool isShiftFlag() const {
            return this->flag & key::FlagShift;
        }
        
        bool isCtrlFlag() const {
            return this->flag & key::FlagCtrl;
        }
        
        bool isAltFlag() const {
            return this->flag & key::AltCtrl;
        }
    };
    
} // namespace ukn

#endif
