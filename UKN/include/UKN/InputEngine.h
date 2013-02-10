#include "UKN/PreDeclare.h"
#include "UKN/Input.h"

#include "mist/Util.h"
#include "mist/Singleton.h"
#include "mist/TimeUtil.h"

namespace ukn {

    enum InputActionType {
        IA_MOUSE_X = 512,
        IA_MOUSE_Y,
        IA_MOUSE_WHEEL,
    };

    struct InputAction {
        /* event, can be values InputAction, input::KeyCode, input::MouseButton, input::JoystickButton */
        uint16 event; 
        /* value, is input::KeyState except when event is InputAction,
            otherwise delta values in position x/y/wheel */
        uint16 value; 

        /* elapsed time in ms, from last event publish to now */
        uint32 elapsed;
    };
   
    class InputEngine: Uncopyable, public mist::Singleton<InputEngine> {
    protected:
        InputEngine();
        virtual ~InputEngine();

    public:

    private:
        void onKeyEvent(Window*, input::KeyEventArgs&);
        void onMouseEvent(Window*, input::JoyStickEventArgs&);
        void onMouseEvent(Window*, input::MouseEventArgs&);

    private:
        mist::FixedArray<uint8, 256> mKeyStatus;
        mist::FixedArray<uint8, 8> mMouseStatus;
        mist::FixedArray<uint8, 16> mJoystickStatus;
    };

}