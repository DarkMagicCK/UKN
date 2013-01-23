
#ifndef Project_Unknown_WindowsWindow_h
#define Project_Unknown_WindowsWindow_h


#include "mist/Platform.h"
#include "UKN/Window.h"
#include "UKN/FrameBuffer.h"

namespace ukn {

	/* General Window(HWND) Class in Windows */
	class WindowsWindow: public Window  {
    public:
        WindowsWindow(const UknString& name, const RenderSettings& settings);
        ~WindowsWindow();
        
        UknString description() const;
        
		HWND getHWnd() const;
        bool pullEvents();
        
        int2  getMousePos();
        int32 getMouseWheel();
        
        bool isKeyDown(input::KeyCode key);
		bool isMouseButtonDown(input::MouseButton btn);
        
        void setMousePos(int32 x, int32 y);

        void onWindowResize(void* wnd, WindowResizeEventArgs&);
        void onWindowMove(Window& wnd, uint32 x, uint32 y);
        
        void setName(const UknString& name);
        
		void setFrameBuffer(const FrameBufferPtr& frameBuffer);
		FrameBufferPtr getFrameBuffer() const;
		
		void updateWindowProperties(int32 x, int32 y, uint32 w, uint32 h);

    private:
		void createWindow(const UknString& name, const RenderSettings& settings);
        
		HINSTANCE mInstance;
		HWND mhWnd;
		FrameBufferPtr mFrameBuffer;
    };

} // namespace ukn

#endif