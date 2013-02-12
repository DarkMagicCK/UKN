#ifndef PROJECT_UNKNOWN_INPUTDEVICE_H_
#define PROJECT_UNKNOWN_INPUTDEVICE_H_

#include "UKN/PreDeclare.h"

namespace ukn {

    class InputDevice: Uncopyable {
    public:
        InputDevice();
        virtual ~InputDevice();

        virtual const UknString& getName() const = 0;
    };

}