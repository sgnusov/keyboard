#include "stm32_keyboard.h"
#include <string>

//#define DEBUG
#define STRIP_PIN PB9
#define NUMLEDs 56
#define NUM_LEDS 60

static const uint8_t IND[60] PROGMEM = {
	56, 56,  3,  2,  1,  0, 31, 30, 29, 28, 56, 56,
	 9,  8,  7,  6,  5,  4, 37, 36, 35, 34, 33, 32,
	15, 14, 13, 12, 11, 10, 43, 42, 41, 40, 39, 38,
	21, 20, 19, 18, 17, 16, 49, 48, 47, 46, 45, 44,
	27, 26, 25, 24, 23, 22, 55, 54, 53, 52, 51, 50,
};

#include "microLED/src/microLED.h"

#define HEIGHT 5
#define WIDTH 12
#define MODES_AMOUNT 10
#define CHANGE_SPEED 300
#define ACCEL 1
#define MAX_SPEED 10
#define LIGHTERS_MODE 8
#define FADE_MODE 9

int8_t currentMode = 0;
byte brightness = 30;
boolean ONflag = false;
boolean loadingFlag = true;
boolean settChanged = false;
unsigned char matrixValue[8][16];

struct Mode {
  byte speed = 30;
  byte scale = 40;
}; 
Mode modes[MODES_AMOUNT];

int8_t curSpeed = 0;
unsigned long lastTime = 0;
unsigned long lastIdle = 0;

byte changeUp(byte cur) {
	if(millis() - lastTime < CHANGE_SPEED)
		return cur;
	lastTime = millis();
	if(curSpeed <= 0)
		curSpeed = 0;
	curSpeed += ACCEL;
	curSpeed = (curSpeed > MAX_SPEED ? MAX_SPEED : curSpeed);
	if(255 - curSpeed < (int16_t)cur)
		return 255;
	return cur + curSpeed;
}

byte changeDown(byte cur) {
	if(millis() - lastTime < CHANGE_SPEED)
		return cur;
	lastTime = millis();
	if(curSpeed >= 0)
		curSpeed = 0;
	curSpeed -= ACCEL;
	curSpeed = (curSpeed < -MAX_SPEED ? -MAX_SPEED : curSpeed);
	if(-curSpeed > (int16_t)cur)
		return 0;
	return cur + curSpeed;
}

void resetChange() {
	curSpeed = 0;
	//lastTime = 0;
}

microLED<NUMLEDs, STRIP_PIN, ORDER_GRB> led;

#define cnth 5
#define cntv 12

int lbtnh = 4, lbtnv = 2;
int pinh[] = {PB12, PB13, PB14, PB15, PA8};
int pinv[] = {PA1, PA2, PA3, PA4, PA5, PA6, PA0, PA7, PB0, PB1, PB10, PB11};

int btn[cnth][cntv] = {
	{ESC, _1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _0_, MNS},
	{TAB, _Q_, _W_, _E_, _R_, _T_, _Y_, _U_, _I_, _O_, _P_, DEL},
	{GRV, _A_, _S_, _D_, _F_, _G_, _H_, _J_, _K_, _L_, SMC, ENT},
	{LSH, _Z_, _X_, _C_, _V_, _B_, _N_, _M_, CMM, DOT, SLS, RSH},
	{  0,   0,   0, LCT, SPC, LWN, INS, SPC, LAL, BSP,   0,   0}
};

int btn1[cnth][cntv] = {
	{F01, F02, F03, F04, F05, F06, F07, F08, F09, F10, F11, F12},
	{TAB, _Q_, PGU, _E_, _R_, _T_, _Y_, _U_, AUP, SBL, SBR, DEL},
	{EQL, HOM, PGD, END, _F_, _G_, _H_, ARG, ADW, ALF, APO, ENT},
	{LSH, _Z_, _X_, _C_, _V_, _B_, _N_, _M_, CMM, DOT, BSL, RSH},
	{  0,   0,   0, LCT, SPC, RWN, INS, SPC, RAL, BSP,   0,   0}
};

int nowbtn[cnth][cntv];
int rea[cnth][cntv];

void setup()
{
#ifdef DEBUG
	Serial1.begin(115200);
	while(!Serial1) {};
#endif
	pinMode(STRIP_PIN, OUTPUT);
	led.clear();
	led.show();
	
	settingsInit();
	led.setBrightness(brightness);
	
	pinMode(PC13, OUTPUT);
	digitalWrite(PC13, HIGH);


	Keyboard.begin();  
	for (int i = 0; i < cnth; ++i)
	{  
		pinMode(pinh[i], INPUT);
		digitalWrite(pinh[i], LOW);
	}
	for (int i = 0; i < cntv; ++i)
	{  
		pinMode(pinv[i], INPUT_PULLUP);
		//digitalWrite(pinv[i], HIGH);
	}
	for (int i = 0; i < cnth; ++i)
		for (int j = 0; j < cntv; ++j)
			nowbtn[i][j] = 0;
	for (int i = 0; i < cnth; ++i)
		for (int j = 0; j < cntv; ++j)
			rea[i][j] = 0;

}

void loop() {
	for(int i = 0; i < cnth; ++i) {
		pinMode(pinh[i], OUTPUT);
		digitalWrite(pinh[i], LOW);
		for (int j = 0; j < cntv; ++j) {
			rea[i][j] = !digitalRead(pinv[j]);
		}
		pinMode(pinh[i], INPUT);
		digitalWrite(pinh[i], LOW);
	}
/*
	for (int i = 0; i < cnth; ++i) {
		for (int j = 0; j < cntv; ++j)
			Serial1.write(std::to_string(rea[i][j]).c_str());
		Serial1.write("\n");
	}
*/
	int lbtn = rea[lbtnh][lbtnv];
	if(lbtn) {
		bool f = true;
		if(rea[1][4])
			modes[currentMode].scale = changeDown(modes[currentMode].scale);
		else if(rea[1][5])
			modes[currentMode].scale = changeUp(modes[currentMode].scale);
		else if(rea[2][4])
			modes[currentMode].speed = changeDown(modes[currentMode].speed);
		else if(rea[2][5])
			modes[currentMode].speed = changeUp(modes[currentMode].speed);
		else if(rea[3][4]) {
			brightness = changeDown(brightness);
			led.setBrightness(brightness);
		} else if(rea[3][5]) {
			brightness = changeUp(brightness);
			led.setBrightness(brightness);
		} else if(rea[1][6] && millis() - lastTime > CHANGE_SPEED) {
			lastTime = millis();
			currentMode += 1;
			currentMode %= MODES_AMOUNT;
			led.clear();
		} else if(rea[1][7] && millis() - lastTime > CHANGE_SPEED) {
			lastTime = millis();
			currentMode += MODES_AMOUNT - 1;
			currentMode %= MODES_AMOUNT;
			led.clear();
		} else if(rea[2][6] && millis() - lastTime > CHANGE_SPEED) {
			lastTime = millis();
			switchPower();
		} else {
			f = false;
		}
		if(f)
			lastIdle = millis();
		else
			settChanged = true;
		if(millis() - lastIdle > CHANGE_SPEED)
			resetChange();
	}
	for(int i = 0; i < cnth; ++i)
		for(int j = 0; j < cntv; ++j) {
			if(!nowbtn[i][j] && rea[i][j]) {
				if(currentMode == LIGHTERS_MODE || currentMode == FADE_MODE)
					press(j, HEIGHT - 1 - i);
				if(lbtn) {
					if((i > 0 && i < 5 && j > 3 && j < 6) || (i == 1 && j > 5 && j < 8) || (i == 2 && j == 6))
						continue;
					nowbtn[i][j] = btn1[i][j];
				} else {
					nowbtn[i][j] = btn[i][j];
				}
				if(nowbtn[i][j]) {
					//Serial1.println((std::to_string(i) + ' ' + std::to_string(j)).c_str());
					nowbtn[i][j] = Keyboard.press(nowbtn[i][j]);
				}
			} else if(nowbtn[i][j] && !rea[i][j]) {
				nowbtn[i][j] = Keyboard.release(nowbtn[i][j]);
			}
		}
	
	Keyboard.sendReport();
	effectsTick();
	eepromTick();
	delay(1);

	//uint32_t pinMask = STM_LL_GPIO_PIN(digitalPinToPinName(PC13));
	//GPIO_TypeDef *port = get_GPIO_Port(STM_PORT(digitalPinToPinName(PC13)));
	//digitalWrite(PC13, HIGH);
	//LL_GPIO_SetOutputPin(port, pinMask);
	//strip.setColor(0, AQUA);
	//strip.show();
	//delay(1000);
	//delayShort(24000000);
	//strip.setColor(0, RED);
	//strip.show();
	//LL_GPIO_ResetOutputPin(port, pinMask);
	//Serial1.println(F_CPU);
	//digitalWrite(PC13, LOW);
	//delay(1000);
}
