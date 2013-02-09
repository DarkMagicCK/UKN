### Input

replace Window::on[Key|Mouse|Joystick]Events with abstract InputDevice

	InputEvent:
		type = Mouse | Key | Joystick
		event = Press | Move | Release 
		data1, data2 = key state
		
	InputDevice:
	    InputMouse
	    InputKeyboard
	    InputJoystick
	    
	InputEngine
		device list -> keyboard, mouse, joystick....
		action map -> uint, uint
		action handler -> signal(for each action in action_map)
			add
			remove
			
			
			
### FTFont

merge multiple texture data into one big texture to accelerate rendering




		