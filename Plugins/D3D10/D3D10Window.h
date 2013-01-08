#include "mist/Platform.h"
#include "UKN/Window.h"

#include "D3D10FrameBuffer.h"

namespace ukn {

	class D3D10Window: public Window  {
    public:
        D3D10Window(const UknString& name, const RenderSettings& settings);
        ~D3D10Window();
        
        UknString description() const;
        
		HWND getHWnd() const;
        bool pullEvents();
        
        int2  getMousePos();
        int32 getMouseWheel();
        
        bool isKeyDown(input::Key::KeyCode key);
        bool isMouseButtonDown(input::Mouse::MouseButton btn);
        
        void setMousePos(int32 x, int32 y);

        void onWindowResize(void* wnd, WindowResizeEventArgs&);
        void onWindowMove(Window& wnd, uint32 x, uint32 y);
        
        void setName(const UknString& name);
        
        D3D10FrameBuffer getFrameBuffer() const;
        
    private:
		void createWindow(const UknString& name, const RenderSettings& settings);
        void updateWindowProperties(int32 x, int32 y, uint32 w, uint32 h);

		HINSTANCE mInstance;
		HWND mhWnd;
        D3D10FrameBuffer mFrameBuffer;
    };

} // namespace ukn