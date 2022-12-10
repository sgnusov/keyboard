#include "stm32_keyboard.h"

#if defined(USBCON)
#include "usbd_hid_composite_if.h"

stm32_keyboard_::stm32_keyboard_(void) 
{
}

void stm32_keyboard_::begin(void)
{
	HID_Composite_Init(HID_KEYBOARD);
}

void stm32_keyboard_::end(void)
{
	HID_Composite_DeInit(HID_KEYBOARD);
}

void stm32_keyboard_::sendReport(void)
{
	uint8_t buf[8] = {_keyReport.modifiers, _keyReport.reserved, _keyReport.keys[0], _keyReport.keys[1],
						_keyReport.keys[2], _keyReport.keys[3], _keyReport.keys[4], _keyReport.keys[5]};
	HID_Composite_keyboard_sendReport(buf, 8);
}


size_t stm32_keyboard_::press(uint8_t code)
{
	if (code >= LCT && code <= RWN) {
		_keyReport.modifiers |= (1 << (code - LCT));
		return code;  
	}

	uint8_t i = 0;
	for (i = 0; i < 6; i++) {
		if (_keyReport.keys[i] == 0x00 || _keyReport.keys[i] == code) {
			_keyReport.keys[i] = code;
			break;
		}
	}
	if (i == 6) {
		return 0;
	}
	return code;
}


size_t stm32_keyboard_::release(uint8_t code) 
{
	if (code >= LCT && code <= RWN) {
		_keyReport.modifiers &= (-1) ^ (1 << (code - LCT));
		return 0;
	}

	uint8_t i = 0;
	for (i = 0; i < 6; i++) {
		if (_keyReport.keys[i] == code) {
			_keyReport.keys[i] = 0;
			break;
		}
	}
	if (i == 6) {
		return code;
	}
	return 0;
}

void stm32_keyboard_::releaseAll(void)
{
	_keyReport.keys[0] = 0;
	_keyReport.keys[1] = 0;	
	_keyReport.keys[2] = 0;
	_keyReport.keys[3] = 0;	
	_keyReport.keys[4] = 0;
	_keyReport.keys[5] = 0;	
	_keyReport.modifiers = 0;
}

stm32_keyboard_ Keyboard;

#endif

