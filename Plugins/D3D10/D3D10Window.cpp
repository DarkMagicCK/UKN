#include "D3D10Window.h"
#include "mist/SysUtil.h"
#include "ukn/Context.h"

namespace ukn {

	input::Key::KeyCode win_keycode_to_ukn(int keyCode) {
		/* keycode in ukn is the standard windows virtuak keycodes */
		return (input::Key::KeyCode)keyCode;
	}

	static int g_key_flag;
    
	static bool is_win_key_down(int key) {
		/* high-order bits == 1 -> key down) */
		return ::GetKeyState(key) & 0x8000;
	}

    static void update_key_flag() {
        g_key_flag = 0;
		if(is_win_key_down(VK_LSHIFT) || is_win_key_down(VK_RSHIFT))
			g_key_flag |= input::Key::FlagShift;
		if(is_win_key_down(VK_LCONTROL) || is_win_key_down(VK_RCONTROL))
			g_key_flag |= input::Key::FlagCtrl;
		if(is_win_key_down(VK_LMENU) || is_win_key_down(VK_RMENU))
			g_key_flag |= input::Key::FlagAlt;
		if(::GetKeyState(VK_CAPITAL) == 1)
			g_key_flag |= input::Key::FlagCapslock;
		if(::GetKeyState(VK_SCROLL) == 1)
			g_key_flag |= input::Key::FlagScrolllock;
		if(::GetKeyState(VK_NUMLOCK) == 1)
			g_key_flag |= input::Key::FlagNumlock;
    }
    
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
		D3D10Window* d3dWindow = (D3D10Window*)GetWindowLongPtr(hWnd, GWL_USERDATA);
		switch (message)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_CLOSE:
			PostQuitMessage(0);
			break;

		case WM_KEYDOWN:
			d3dWindow->onKeyEvent().raise(
				d3dWindow, 
				input::KeyEventArgs(input::Key::KeyState::Press, win_keycode_to_ukn(wParam),
				g_key_flag)
			);
			break;

		case WM_KEYUP:
			d3dWindow->onKeyEvent().raise(
				d3dWindow,
				input::KeyEventArgs(input::Key::KeyState::Release, win_keycode_to_ukn(wParam), 
				g_key_flag)
			);
			break;

		case WM_MOUSEMOVE: {
			int2 pos = d3dWindow->getMousePos();
			d3dWindow->onMouseEvent().raise(
				d3dWindow,
				input::MouseEventArgs(input::Mouse::MouseState::Move,
									  input::Mouse::MouseButton::Null,
									  g_key_flag,
									  pos[0],
									  pos[1],
									  GET_WHEEL_DELTA_WPARAM(wParam))
            );
			break;
		}

		case WM_MOUSEWHEEL: {
			int2 pos = d3dWindow->getMousePos();
			d3dWindow->onMouseEvent().raise(
				d3dWindow,
				input::MouseEventArgs(input::Mouse::MouseState::Wheel,
									  input::Mouse::MouseButton::Null,
									  g_key_flag,
									  pos[0],
									  pos[1],
									  GET_WHEEL_DELTA_WPARAM(wParam))
            );
			break;
		}

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP: {
			int2 pos = d3dWindow->getMousePos();
			d3dWindow->onMouseEvent().raise(
				d3dWindow,
				input::MouseEventArgs(message == WM_LBUTTONDOWN ? input::Mouse::MouseState::Press : input::Mouse::MouseState::Release,
									  input::Mouse::MouseButton::LeftButton,
									  g_key_flag,
									  pos[0],
									  pos[1],
									  GET_WHEEL_DELTA_WPARAM(wParam))
            );
			break;
		}

		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP: {
			int2 pos = d3dWindow->getMousePos();
			d3dWindow->onMouseEvent().raise(
				d3dWindow,
				input::MouseEventArgs(message == WM_RBUTTONDOWN ? input::Mouse::MouseState::Press : input::Mouse::MouseState::Release,
									  input::Mouse::MouseButton::RightButton,
									  g_key_flag,
									  pos[0],
									  pos[1],
									  GET_WHEEL_DELTA_WPARAM(wParam))
            );
			break;
		}

		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP: {
			int2 pos = d3dWindow->getMousePos();
			d3dWindow->onMouseEvent().raise(
				d3dWindow,
				input::MouseEventArgs(message == WM_MBUTTONDOWN ? input::Mouse::MouseState::Press : input::Mouse::MouseState::Release,
									  input::Mouse::MouseButton::MiddleButton,
									  g_key_flag,
									  pos[0],
									  pos[1],
									  GET_WHEEL_DELTA_WPARAM(wParam))
            );
			break;
	    }
		}
	}
	
	D3D10Window::D3D10Window(const UknString& name, const RenderSettings& settings):
	ukn::Window(name),
	mFrameBuffer(new D3D10FrameBuffer(false)) {
		this->createWindow(name, settings);
	}

    D3D10Window::~D3D10Window() {
		if(ukn::Context::Instance().getCfg().render_cfg.full_screen)
			ChangeDisplaySettings(0, 0);

		DestroyWindow(mhWnd);
		UnregisterClassW(this->getWindowTitle().c_str(), mInstance);
	}

	void D3D10Window::createWindow(const UknString& name, const RenderSettings& settings) {
		WNDCLASSEXW wc;

		mInstance = GetModuleHandle(0);

		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = mInstance;
		if(settings.icon.length() > 0) 
			wc.hIcon = ::LoadIconW(mInstance, settings.icon.c_str());
		else
			wc.hIcon = 0;
		if(settings.cursor.length() > 0)
			wc.hCursor = ::LoadCursor(mInstance, settings.cursor.c_str());
		else
			wc.hCursor = 0;
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = name.c_str();
		wc.cbSize = sizeof(WNDCLASSEXW);

		RegisterClassExW(&wc);

		if(settings.full_screen) {
			/* in fullscreen mode, reset current screen settings */
			DEVMODE screenSettings;
			memset(&screenSettings, 0, sizeof(screenSettings));

			screenSettings.dmSize = sizeof(screenSettings);
			screenSettings.dmPelsWidth = (unsigned long)settings.width;
			screenSettings.dmPelsHeight = (unsigned long)settings.height;
			screenSettings.dmBitsPerPel = 32;
			screenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			if(::ChangeDisplaySettingsW(&screenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
				MIST_THROW_EXCEPTION("Error change screen settings to fullscreen");
			}
		}

		mhWnd = ::CreateWindowExW(WS_EX_APPWINDOW,
			                      name.c_str(),
								  name.c_str(),
								  WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
								  settings.full_screen ? 0 : (GetSystemMetrics(SM_CXSCREEN) - settings.width) / 2,
								  settings.full_screen ? 0 : (GetSystemMetrics(SM_CYSCREEN) - settings.height) / 2,
								  settings.width,
								  settings.height,
								  0,
								  0,
								  mInstance,
								  0);
		if(mhWnd == NULL) {
			MIST_THROW_EXCEPTION("Error create window");
		}
		SetWindowLongPtr(mhWnd, GWL_USERDATA, (LONG_PTR)this);
		ShowWindow(mhWnd, SW_SHOW);
		SetForegroundWindow(mhWnd);
		SetFocus(mhWnd);

		ShowCursor(settings.show_mouse);
	}
        
	UknString D3D10Window::description() const {
		return L"D3D10 Window";
	}
        
	HWND D3D10Window::getHWnd() const {
		return mhWnd;
	}

	bool D3D10Window::pullEvents() {
		bool gotMsg;
		MSG  msg;
        
		while(::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
        
			if (WM_QUIT != msg.message && WM_CLOSE != msg.message) {
				gotMsg = ::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ? true : false;
				
				if (gotMsg)
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			} else 
				return true;
		}
        update_key_flag();
		return false;
	}
        
	int2 D3D10Window::getMousePos() {
		POINT p;
		if(::GetCursorPos(&p)) {
			if(::ScreenToClient(mhWnd, &p))
				return int2(p.x, p.y);
		}
		return int2(0, 0);
	}

	int32 D3D10Window::getMouseWheel() {
		return 0;
	}
        
	bool D3D10Window::isKeyDown(input::Key::KeyCode key) {
		return is_win_key_down(key);
	}

	bool D3D10Window::isMouseButtonDown(input::Mouse::MouseButton btn) {
		switch(btn) {
		case input::Mouse::LeftButton:
			return ::GetAsyncKeyState(VK_LBUTTON) & 0x8000;
		case input::Mouse::RightButton:
			return ::GetAsyncKeyState(VK_RBUTTON) & 0x8000;
		case input::Mouse::MiddleButton:
			return ::GetAsyncKeyState(VK_MBUTTON) & 0x8000;
		}
		return false;
	}
        
	void D3D10Window::setMousePos(int32 x, int32 y) {
		POINT p;
		p.x = x;
		p.y = y;
		if(::ClientToScreen(mhWnd, &p)) {
			::SetCursorPos(p.x, p.y);
		}
	}

	void D3D10Window::onWindowResize(void* wnd, WindowResizeEventArgs& args) {
		  updateWindowProperties(left(), top(), args.width, args.height);
	}

	void D3D10Window::onWindowMove(Window& wnd, uint32 x, uint32 y) {
		 updateWindowProperties(x, y, width(), height());
	}
        
	void D3D10Window::setName(const UknString& name) {
		::SetWindowTextW(mhWnd, name.c_str());
	}
        
	D3D10FrameBuffer D3D10Window::getFrameBuffer() const {
		return mFrameBuffer;
	}
        
	void D3D10Window::updateWindowProperties(int32 x, int32 y, uint32 w, uint32 h) {
	/*	mFrameBuffer->mLeft = 0;
        mFrameBuffer->mTop = 0;
        mFrameBuffer->mWidth = w;
        mFrameBuffer->mHeight = h;
        
        Window::mLeft = x;
        Window::mTop = y;
        Window::mWidth = w;
        Window::mHeight = h;
        
        mFrameBuffer->mViewPort.left = 0;
        mFrameBuffer->mViewPort.top = 0;
        mFrameBuffer->mViewPort.width = w;
        mFrameBuffer->mViewPort.height = h;
        
        Viewport& vp = mFrameBuffer->getViewport();
        vp.camera = MakeSharedPtr<Camera2D>();
        ((Camera2D*)vp.camera.get())->setOrthoParams(0, mFrameBuffer->width(), mFrameBuffer->height(), 0);

        vp.camera->update();*/
	}


} // namespace ukn