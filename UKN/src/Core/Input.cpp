//
//  Input.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/24/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "UKN/Input.h"

namespace ukn {
    
    namespace input {
        
        bool KeyEventArgs::isKeyDown(KeyCode k) const {
            return (k == this->key && state == Press);
        }
        
        bool KeyEventArgs::isKeyUp(KeyCode k) const {
            return (k == this->key && state == Release);
        }
        
        bool KeyEventArgs::isDigit() const {
            return this->key >= 48 && this->key <= 57;
        }
        
        bool KeyEventArgs::isLetter() const {
            return (((this->key >= 65 && this->key <= 90)
                     || (this->key >= 97 && this->key <= 122)
                     || (this->key >= 192 && this->key <= 255))
                    && (this->key != 215) && (this->key != 247));
        }
        
        wchar_t KeyEventArgs::toCharCode() const {
            if(isPrintable()) {
                return (wchar_t)key;
            }
            return '?';
        }
        
        bool KeyEventArgs::isPrintable() const {
            if((this->key >= Num0 && key <= Num9) ||
               (this->key >= A && key <= Z) ||
               (this->key == Grave) ||
               (this->key == Minus) ||
               (this->key == Equals) ||
               (this->key == Backslash) ||
               (this->key == LeftBracket) ||
               (this->key == RightBracket) ||
               (this->key == Semicolon) ||
               (this->key == Apostrophe) ||
               (this->key == Comma) ||
               (this->key == Period) ||
               (this->key == Slash) ||
               (this->key == Space)
               )
                return true;
            return false;
        }
        
        bool KeyEventArgs::isShiftFlag() const {
            return this->flag & FlagShift;
        }
        
        bool KeyEventArgs::isCtrlFlag() const {
            return this->flag & FlagCtrl;
        }
        
        bool KeyEventArgs::isAltFlag() const {
            return this->flag & FlagAlt;
        }

    } // namespace input
} // namespace ukn