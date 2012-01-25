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
        
        bool KeyEventArgs::isKeyDown(Key::KeyCode k) const {
            return (k == this->key && state == Key::Press);
        }
        
        bool KeyEventArgs::isKeyUp(Key::KeyCode k) const {
            return (k == this->key && state == Key::Release);
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
        
        char KeyEventArgs::toCharCode() const {
            if(isPrintable()) {
                return (char)key;
            }
            return '?';
        }
        
        bool KeyEventArgs::isPrintable() const {
            if((this->key >= Key::Num0 && key <= Key::Num9) ||
               (this->key >= Key::A && key <= Key::Z) ||
               (this->key == Key::Grave) ||
               (this->key == Key::Minus) ||
               (this->key == Key::Equals) ||
               (this->key == Key::Backslash) ||
               (this->key == Key::LeftBracket) ||
               (this->key == Key::RightBracket) ||
               (this->key == Key::Semicolon) ||
               (this->key == Key::Apostrophe) ||
               (this->key == Key::Comma) ||
               (this->key == Key::Period) ||
               (this->key == Key::Slash) ||
               (this->key == Key::Space)
               )
                return true;
            return false;
        }
        
        bool KeyEventArgs::isShiftFlag() const {
            return this->flag & Key::FlagShift;
        }
        
        bool KeyEventArgs::isCtrlFlag() const {
            return this->flag & Key::FlagCtrl;
        }
        
        bool KeyEventArgs::isAltFlag() const {
            return this->flag & Key::FlagAlt;
        }

    } // namespace input
} // namespace ukn