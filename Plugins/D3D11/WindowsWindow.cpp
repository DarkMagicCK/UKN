
#include "WindowsWindow.h"
#include "mist/SysUtil.h"
#include "ukn/Context.h"

namespace ukn {

	input::KeyCode win_keycode_to_ukn(int keyCode) {
		/* keycode in ukn is the standard windows virtuak keycodes */
		return (input::KeyCode)keyCode;
	}

	static int g_key_flag;
    
	static bool is_win_key_down(int key) {
		/* high-order bits == 1 -> key down) */
		return ::GetKeyState(key) & 0x8000;
	}

    static void update_key_flag() {
        g_key_flag = 0;
		if(is_win_key_down(VK_LSHIFT) || is_win_key_down(VK_RSHIFT))
			g_key_flag |= input::FlagShift;
		if(is_win_key_down(VK_LCONTROL) || is_win_key_down(VK_RCONTROL))
			g_key_flag |= input::FlagCtrl;
		if(is_win_key_down(VK_LMENU) || is_win_key_down(VK_RMENU))
			g_key_flag |= input::FlagAlt;
		if(::GetKeyState(VK_CAPITAL) == 1)
			g_key_flag |= input::FlagCapslock;
		if(::GetKeyState(VK_SCROLL) == 1)
			g_key_flag |= input::FlagScrolllock;
		if(::GetKeyState(VK_NUMLOCK) == 1)
			g_key_flag |= input::FlagNumlock;
    }
    
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
		WindowsWindow* window = (WindowsWindow*)GetWindowLongPtr(hWnd, GWL_USERDATA);
		switch (message) {
			case WM_DESTROY:
				PostQuitMessage(0);
				break;

			case WM_CLOSE:
				PostQuitMessage(0);
				break;

			case WM_KEYDOWN:
				window->onKeyEvent().raise(
					window, 
					input::KeyEventArgs(input::Press, win_keycode_to_ukn(wParam),
					g_key_flag)
				);
				break;

			case WM_IME_CHAR: {
				input::KeyEventArgs args(input::Press, (mist::uint16)wParam, g_key_flag);
				args.isIME = true;
				window->onKeyEvent().raise(
					window, 
					args
				);
				break;
			}

			case WM_KEYUP:
				window->onKeyEvent().raise(
					window,
					input::KeyEventArgs(input::Release, win_keycode_to_ukn(wParam), 
					g_key_flag)
				);
				break;

			case WM_MOUSEMOVE: {
				int2 pos = window->getMousePos();
				window->onMouseEvent().raise(
					window,
					input::MouseEventArgs(input::Move,
										  input::Nothing,
										  g_key_flag,
										  pos[0],
										  pos[1],
										  GET_WHEEL_DELTA_WPARAM(wParam))
				);
				break;
			}

			case WM_MOUSEWHEEL: {
				int2 pos = window->getMousePos();
				window->onMouseEvent().raise(
					window,
					input::MouseEventArgs(input::Wheel,
										  input::Nothing,
										  g_key_flag,
										  pos[0],
										  pos[1],
										  GET_WHEEL_DELTA_WPARAM(wParam))
				);
				break;
			}

			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP: {
				int2 pos = window->getMousePos();
				window->onMouseEvent().raise(
					window,
					input::MouseEventArgs(message == WM_LBUTTONDOWN ? input::Press : input::Release,
										  input::LeftButton,
										  g_key_flag,
										  pos[0],
										  pos[1],
										  GET_WHEEL_DELTA_WPARAM(wParam))
				);
				break;
			}

			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP: {
				int2 pos = window->getMousePos();
				window->onMouseEvent().raise(
					window,
					input::MouseEventArgs(message == WM_RBUTTONDOWN ? input::Press : input::Release,
										  input::MouseButton::RightButton,
										  g_key_flag,
										  pos[0],
										  pos[1],
										  GET_WHEEL_DELTA_WPARAM(wParam))
				);
				break;
			}

			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP: {
				int2 pos = window->getMousePos();
				window->onMouseEvent().raise(
					window,
					input::MouseEventArgs(message == WM_MBUTTONDOWN ? input::Press : input::Release,
										  input::MouseButton::MiddleButton,
										  g_key_flag,
										  pos[0],
										  pos[1],
										  GET_WHEEL_DELTA_WPARAM(wParam))
				);
				break;
			}
		}
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}
	
	WindowsWindow::WindowsWindow(const UknString& name, const RenderSettings& settings):
	ukn::Window(name) {
		this->createWindow(name, settings);
	}

    WindowsWindow::~WindowsWindow() {
		if(ukn::Context::Instance().getCfg().render_cfg.full_screen)
			ChangeDisplaySettings(0, 0);

		DestroyWindow(mhWnd);
		UnregisterClassW(this->getWindowTitle().c_str(), mInstance);
	}

	void WindowsWindow::createWindow(const UknString& name, const RenderSettings& settings) {
		WNDCLASSEXW wc;

		mInstance = GetModuleHandle(0);

		ZeroMemory(&wc, sizeof(WNDCLASSEXW));
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
				MIST_THROW_EXCEPTION(L"Error change screen settings to fullscreen");
			}
		}

		DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		DWORD exStyle = WS_EX_APPWINDOW;
		if(settings.resizable) {
			style |= WS_SIZEBOX | WS_MAXIMIZEBOX;
			exStyle |= WS_EX_WINDOWEDGE;
		}
		mhWnd = ::CreateWindowExW(exStyle,
			                      name.c_str(),
								  name.c_str(),
								  style,
								  settings.full_screen ? 0 : (GetSystemMetrics(SM_CXSCREEN) - settings.width) / 2,
								  settings.full_screen ? 0 : (GetSystemMetrics(SM_CYSCREEN) - settings.height) / 2,
								  settings.width,
								  settings.height,
								  0,
								  0,
								  mInstance,
								  0);
		if(mhWnd == NULL) {
			MIST_THROW_EXCEPTION(L"Error create window");
		}
		SetWindowLongPtr(mhWnd, GWL_USERDATA, (LONG_PTR)this);
		ShowWindow(mhWnd, SW_SHOW);
		SetForegroundWindow(mhWnd);
		SetFocus(mhWnd);

		ShowCursor(settings.show_mouse);
	}
        
	UknString WindowsWindow::description() const {
		return L"Windows Window";
	}
        
	HWND WindowsWindow::getHWnd() const {
		return mhWnd;
	}

	bool WindowsWindow::pullEvents() {
		bool gotMsg;
		MSG  msg;
        
		while(::PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			if (WM_QUIT != msg.message && WM_CLOSE != msg.message) {
				gotMsg = ::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE) == TRUE ? true : false;
				
				if (gotMsg) {
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			} else 
				return true;
		}
        update_key_flag();
		return false;
	}
        
	int2 WindowsWindow::getMousePos() {
		POINT p;
		if(::GetCursorPos(&p)) {
			if(::ScreenToClient(mhWnd, &p))
				return int2(p.x, p.y);
		}
		return int2(0, 0);
	}

	int32 WindowsWindow::getMouseWheel() {
		return 0;
	}
        
	bool WindowsWindow::isKeyDown(input::KeyCode key) {
		return is_win_key_down(key);
	}

	bool WindowsWindow::isMouseButtonDown(input::MouseButton btn) {
		switch(btn) {
		case input::LeftButton:
			return ::GetAsyncKeyState(VK_LBUTTON) & 0x8000;
		case input::RightButton:
			return ::GetAsyncKeyState(VK_RBUTTON) & 0x8000;
		case input::MiddleButton:
			return ::GetAsyncKeyState(VK_MBUTTON) & 0x8000;
		}
		return false;
	}
        
	void WindowsWindow::setMousePos(int32 x, int32 y) {
		POINT p;
		p.x = x;
		p.y = y;
		if(::ClientToScreen(mhWnd, &p)) {
			::SetCursorPos(p.x, p.y);
		}
	}

	void WindowsWindow::onWindowResize(void* wnd, WindowResizeEventArgs& args) {
		  updateWindowProperties(left(), top(), args.width, args.height);
	}

	void WindowsWindow::onWindowMove(Window& wnd, uint32 x, uint32 y) {
		 updateWindowProperties(x, y, width(), height());
	}
        
	void WindowsWindow::setName(const UknString& name) {
		::SetWindowTextW(mhWnd, name.c_str());
	}

	FrameBufferPtr WindowsWindow::getFrameBuffer() const {
		return mFrameBuffer;
	}

	void WindowsWindow::setFrameBuffer(const FrameBufferPtr& frameBuffer) {
		mFrameBuffer = frameBuffer;
	}
       
	void WindowsWindow::updateWindowProperties(int32 x, int32 y, uint32 w, uint32 h) {
        Window::mLeft = x;
        Window::mTop = y;
        Window::mWidth = w;
        Window::mHeight = h;
        
		if(mFrameBuffer) {
			mFrameBuffer->updateScreenSize(0, 0, w, h);

			Viewport& vp = mFrameBuffer->getViewport();
        
		    vp.left = 0;
		    vp.top = 0;
		    vp.width = w;
		    vp.height = h;
        
			vp.camera = MakeSharedPtr<Camera>();
		    vp.camera->setProjParams(d_pi / 2,  (float)mFrameBuffer->width() / (float)mFrameBuffer->height(), 0.1, 100.f);
			vp.camera->setViewParams(Vector3(0, 0, -5), Vector3(0, 0, 1));
		}
	}


} // namespace ukn
