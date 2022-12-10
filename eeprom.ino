#include <EEPROM.h>

#define SHIFT 0
/*
0 byte - currentMode
1 byte - brightness
2 byte - ONflag
3 byte - INIT value
*/

uint32_t eepromTimer = 0;

void saveEEPROM() {
	EEPROM.put(sizeof(Mode) * currentMode + 4 + SHIFT, modes[currentMode]);
	//EEPROM.commit();
}

void eepromTick() {
	if(settChanged && millis() - eepromTimer > 1000) {
#ifdef DEBUG
		Serial1.println("EepromTick");
		Serial1.println(EEPROM.read(3 + SHIFT));
#endif
		settChanged = false;
		eepromTimer = millis();
		saveEEPROM();
		if(EEPROM.read(0 + SHIFT) != currentMode) EEPROM.write(0 + SHIFT, currentMode);
		if(EEPROM.read(1 + SHIFT) != brightness) EEPROM.write(1 + SHIFT, brightness);
		if(EEPROM.read(2 + SHIFT) != ONflag) EEPROM.write(2 + SHIFT, ONflag);
		//EEPROM.commit();
	}
}

void settingsInit() {
#ifndef DEBUG
	if(EEPROM.read(3 + SHIFT) != 1) {
#else
	if(true) {
		Serial1.println("Init");
		Serial1.println(EEPROM.read(3 + SHIFT));
#endif
		EEPROM.write(0 + SHIFT, currentMode);
		EEPROM.write(1 + SHIFT, brightness);
		EEPROM.write(2 + SHIFT, ONflag);
		EEPROM.write(3 + SHIFT, 1);
		for(byte i = 0; i < MODES_AMOUNT; ++i)
			EEPROM.put(sizeof(Mode) * currentMode + 4 + SHIFT, modes[i]);
		//EEPROM.commit();
	} else {
#ifdef DEBUG
		Serial1.println("Loading");
		Serial1.println(EEPROM.read(3 + SHIFT));
#endif
		EEPROM.get(0 + SHIFT, currentMode);
		EEPROM.get(1 + SHIFT, brightness);
		EEPROM.get(2 + SHIFT, ONflag);
		for(byte i = 0; i < MODES_AMOUNT; ++i)
			EEPROM.get(sizeof(Mode) * currentMode + 4 + SHIFT, modes[i]);
	}
}
