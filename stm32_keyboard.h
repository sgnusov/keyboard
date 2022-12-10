#ifndef STM32_KEYBOARD_h
#define STM32_KEYBOARD_h

#include <Arduino.h>

#if !defined(USBCON) || !defined(USBD_USE_HID_COMPOSITE)

#error "USB HID not enabled! Select 'HID' in the 'Tools->USB interface' menu."

#else


#define _1_              0x1E
#define _2_              0x1F
#define _3_              0x20
#define _4_              0x21
#define _5_              0x22
#define _6_              0x23
#define _7_              0x24
#define _8_              0x25
#define _9_              0x26
#define _0_              0x27

#define _A_              0x04
#define _B_              0x05
#define _C_              0x06
#define _D_              0x07
#define _E_              0x08
#define _F_              0x09
#define _G_              0x0A
#define _H_              0x0B
#define _I_              0x0C
#define _J_              0x0D
#define _K_              0x0E
#define _L_              0x0F
#define _M_              0x10
#define _N_              0x11
#define _O_              0x12
#define _P_              0x13
#define _Q_              0x14
#define _R_              0x15
#define _S_              0x16
#define _T_              0x17
#define _U_              0x18
#define _V_              0x19
#define _W_              0x1A
#define _X_              0x1B
#define _Y_              0x1C
#define _Z_              0x1D

#define CMM 0x36 // ,
#define DOT 0x37 // .
#define MNS 0x2D // -
#define EQL 0x2E // =
#define BSL 0x31 // back slash
#define SBL 0x2F // [
#define SBR 0x30 // ]
#define SLS 0x38 // /
#define APO 0x34 // '
#define SMC 0x33 // ;
#define GRV 0x35 // ` and ~

#define LCT 0xE0
#define LSH 0xE1
#define LAL 0xE2
#define LWN 0xE3
#define RCT 0xE4
#define RSH 0xE5
#define RAL 0xE6
#define RWN 0xE7

#define AUP 0x52
#define ALF 0x50
#define ARG 0x4F
#define ADW 0x51

#define TAB 0x2B
#define ESC 0x29
#define SPC 0x2C
#define BSP 0x2A
#define SCR 0x46
#define ENT 0x28

#define DEL 0x4C
#define INS 0x49
#define HOM 0x4A
#define END 0x4D
#define PGU 0x4B
#define PGD 0x4E

#define F01 0x3A
#define F02 0x3B
#define F03 0x3C
#define F04 0x3D
#define F05 0x3E
#define F06 0x3F
#define F07 0x40
#define F08 0x41
#define F09 0x42
#define F10 0x43
#define F11 0x44
#define F12 0x45


//  Low level key report: up to 6 keys and shift, ctrl etc at once
typedef struct
{
	uint8_t modifiers;
	uint8_t reserved;
	uint8_t keys[6];
} KeyReport;

class stm32_keyboard_
{
private:
	KeyReport _keyReport;
public:
	stm32_keyboard_(void);
	void begin(void);
	void end(void);
	size_t press(uint8_t k);
	size_t release(uint8_t k);
	size_t write(uint8_t k);
	void releaseAll(void);
	void sendReport();
};
extern stm32_keyboard_ Keyboard;

#endif
#endif

