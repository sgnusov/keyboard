uint32_t effTimer;

void effectsTick() {
  if (ONflag && (millis() - effTimer >= modes[currentMode].speed)) {
    effTimer = millis();
    switch (currentMode) {
      case 0: sparklesRoutine();
        break;
      case 1: fireRoutine();
        break;
      case 2: rainbowVertical();
        break;
      case 3: rainbowHorizontal();
        break;
      case 4: colorsRoutine();
        break;
      case 5: colorRoutine();
        break;
      case 6: snowRoutine();
        break;
      case 7: matrixRoutine();
        break;
      case 8: lightersRoutine();
        break;
	  case 9: FadeRoutine();
	    break;
    }
    led.show();
  }
}

void switchPower() {
	if(ONflag) {
		ONflag = false;
		led.clear();
		led.show();
	} else {
		ONflag = true;
	}
}

/*
void changePower() {
  if (ONflag) {
    effectsTick();
    for (int i = 0; i < modes[currentMode].brightness; i += 8) {
      led.setBrightness(i);
      delay(1);
      led.show();
    }
    led.setBrightness(modes[currentMode].brightness);
    delay(2);
    led.show();
  } else {
    effectsTick();
    for (int i = modes[currentMode].brightness; i > 8; i -= 8) {
      led.setBrightness(i);
      delay(1);
      led.show();
    }
    led.clear();
    delay(2);
    led.show();
  }
}
*/
