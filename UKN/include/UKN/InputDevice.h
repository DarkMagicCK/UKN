#include "UKN/PreDeclare.h"


namespace ukn {

    class InputDevice: Uncopyable {
    public:
        InputDevice();
        virtual ~InputDevice();

        virtual const UknString& getName() const = 0;
    };

}