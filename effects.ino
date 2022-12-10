// ================================= ЭФФЕКТЫ ====================================

mData null(0, 0, 0);
// --------------------------------- конфетти ------------------------------------
void sparklesRoutine() {
  for (byte i = 0; i < modes[0].scale; i++) {
    byte x = random(0, WIDTH);
    byte y = random(0, HEIGHT);
    if (led.get(x, y) == null)
      led.set(x, y, mHSV(random(0, 255), 255, 255));
  }
  fader(70);
}

// функция плавного угасания цвета для всех пикселей
void fader(byte step) {
  for (byte i = 0; i < WIDTH; i++) {
    for (byte j = 0; j < HEIGHT; j++) {
      fadePixel(i, j, step);
    }
  }
}
void fadePixel(byte i, byte j, byte step) {     // новый фейдер
  if (led.get(i, j) == null) return;
  mData color = led.get(i, j);
  if (color.r >= 30 ||
      color.g >= 30 ||
      color.b >= 30) {
    led.fade(i, j, step);
  } else {
    led.set(i, j, null);
  }
}

// -------------------------------------- огонь ---------------------------------------------
// эффект "огонь"
#define SPARKLES 1        // вылетающие угольки вкл выкл
unsigned char line[WIDTH];
int pcnt = 0;

//these values are substracetd from the generated values to give a shape to the animation
const unsigned char valueMask[8][16] PROGMEM = {
  {32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 , 32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 },
  {64 , 0  , 0  , 0  , 0  , 0  , 0  , 64 , 64 , 0  , 0  , 0  , 0  , 0  , 0  , 64 },
  {96 , 32 , 0  , 0  , 0  , 0  , 32 , 96 , 96 , 32 , 0  , 0  , 0  , 0  , 32 , 96 },
  {128, 64 , 32 , 0  , 0  , 32 , 64 , 128, 128, 64 , 32 , 0  , 0  , 32 , 64 , 128},
  {160, 96 , 64 , 32 , 32 , 64 , 96 , 160, 160, 96 , 64 , 32 , 32 , 64 , 96 , 160},
  {192, 128, 96 , 64 , 64 , 96 , 128, 192, 192, 128, 96 , 64 , 64 , 96 , 128, 192},
  {255, 160, 128, 96 , 96 , 128, 160, 255, 255, 160, 128, 96 , 96 , 128, 160, 255},
  {255, 192, 160, 128, 128, 160, 192, 255, 255, 192, 160, 128, 128, 160, 192, 255}
};

//these are the hues for the fire,
//should be between 0 (red) to about 25 (yellow)
const unsigned char hueMask[8][16] PROGMEM = {
  {1 , 11, 19, 25, 25, 22, 11, 1 , 1 , 11, 19, 25, 25, 22, 11, 1 },
  {1 , 8 , 13, 19, 25, 19, 8 , 1 , 1 , 8 , 13, 19, 25, 19, 8 , 1 },
  {1 , 8 , 13, 16, 19, 16, 8 , 1 , 1 , 8 , 13, 16, 19, 16, 8 , 1 },
  {1 , 5 , 11, 13, 13, 13, 5 , 1 , 1 , 5 , 11, 13, 13, 13, 5 , 1 },
  {1 , 5 , 11, 11, 11, 11, 5 , 1 , 1 , 5 , 11, 11, 11, 11, 5 , 1 },
  {0 , 1 , 5 , 8 , 8 , 5 , 1 , 0 , 0 , 1 , 5 , 8 , 8 , 5 , 1 , 0 },
  {0 , 0 , 1 , 5 , 5 , 1 , 0 , 0 , 0 , 0 , 1 , 5 , 5 , 1 , 0 , 0 },
  {0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 }
};

void fireRoutine() {
  if (loadingFlag) {
    loadingFlag = false;
    //FastLED.clear();
    generateLine();
  }
  if (pcnt >= 100) {
    shiftUp();
    generateLine();
    pcnt = 0;
  }
  drawFrame(pcnt);
  pcnt += 30;
}

// Randomly generate the next line (matrix row)

void generateLine() {
  for (uint8_t x = 0; x < WIDTH; x++) {
    line[x] = random(64, 255);
  }
}

void shiftUp() {
  for (uint8_t y = HEIGHT - 1; y > 0; y--) {
    for (uint8_t x = 0; x < WIDTH; x++) {
      uint8_t newX = x;
      if (x > 15) newX = x - 15;
      if (y > 7) continue;
      matrixValue[y][newX] = matrixValue[y - 1][newX];
    }
  }

  for (uint8_t x = 0; x < WIDTH; x++) {
    uint8_t newX = x;
    if (x > 15) newX = x - 15;
    matrixValue[0][newX] = line[newX];
  }
}

// draw a frame, interpolating between 2 "key frames"
// @param pcnt percentage of interpolation

void drawFrame(int pcnt) {
  int nextv;

  //each row interpolates with the one before it
  for (unsigned char y = HEIGHT - 1; y > 0; y--) {
    for (unsigned char x = 0; x < WIDTH; x++) {
      uint8_t newX = x;
      if (x > 15) newX = x - 15;
      if (y < 8) {
        nextv =
          (((100.0 - pcnt) * matrixValue[y][newX]
            + pcnt * matrixValue[y - 1][newX]) / 100.0)
          - pgm_read_byte(&(valueMask[y][newX]));

        mData color = mHSV(
                       modes[1].scale * 2.5 + pgm_read_byte(&(hueMask[y][newX])), // H
                       255, // S
                       (uint8_t)max(0, nextv) // V
                     );

        led.set(x, y, color);
      } else if (y == 8 && SPARKLES) {
        if (random(0, 20) == 0 && led.get(x, y - 1) != null) led.set(x, y, led.get(x, y - 1));
        else led.set(x, y, 0);
      } else if (SPARKLES) {

        // старая версия для яркости
        if (led.get(x, y - 1) != null)
          led.set(x, y, led.get(x, y - 1));
        else led.set(x, y, 0);

      }
    }
  }

  //first row interpolates with the "next" line
  for (unsigned char x = 0; x < WIDTH; x++) {
    uint8_t newX = x;
    if (x > 15) newX = x - 15;
	mData color = mHSV(
                   modes[1].scale * 2.5 + pgm_read_byte(&(hueMask[0][newX])), // H
                   255,           // S
                   (uint8_t)(((100.0 - pcnt) * matrixValue[0][newX] + pcnt * line[newX]) / 100.0) // V
                 );
    led.set(newX, 0, color);
  }
}

byte hue;
// ---------------------------------------- радуга ------------------------------------------
void rainbowVertical() {
  hue += 2;
  for (byte j = 0; j < HEIGHT; j++) {
    mData thisColor = mHSV((byte)(hue + j * modes[2].scale), 255, 255);
    for (byte i = 0; i < WIDTH; i++)
      led.set(i, j, thisColor);
  }
}
void rainbowHorizontal() {
  hue += 2;
  for (byte i = 0; i < WIDTH; i++) {
    mData thisColor = mHSV((byte)(hue + i * modes[3].scale), 255, 255);
    for (byte j = 0; j < HEIGHT; j++)
      led.set(i, j, thisColor);   //leds[getPixelNumber(i, j)] = thisColor;
  }
}

// ---------------------------------------- ЦВЕТА ------------------------------------------
void colorsRoutine() {
  hue += modes[4].scale;
  for (int i = 0; i < NUM_LEDS; i++) {
    led.set(i, mHSV(hue, 255, 255));
  }
}

// --------------------------------- ЦВЕТ ------------------------------------
void colorRoutine() {
  for (int i = 0; i < NUM_LEDS; i++) {
    led.set(i, mHSV(modes[5].scale * 2.5, 255, 255));
  }
}

// ------------------------------ снегопад 2.0 --------------------------------
void snowRoutine() {
  // сдвигаем всё вниз
  for (byte x = 0; x < WIDTH; x++) {
    for (byte y = 0; y < HEIGHT - 1; y++) {
      led.set(x, y, led.get(x, y + 1));
    }
  }

  for (byte x = 0; x < WIDTH; x++) {
    // заполняем случайно верхнюю строку
    // а также не даём двум блокам по вертикали вместе быть
    if (led.get(x, HEIGHT - 2) == null && (random(0, modes[6].scale) == 0))
      led.set(x, HEIGHT - 1, 0xE0FFFF - 0x101010 * random(0, 4));
    else
      led.set(x, HEIGHT - 1, 0x000000);
  }
}

// ------------------------------ МАТРИЦА ------------------------------
void matrixRoutine() {
  for (byte x = 0; x < WIDTH; x++) {
    // заполняем случайно верхнюю строку
    uint32_t thisColor = getHEX(led.get(x, HEIGHT - 1));
    if (thisColor == 0)
      led.set(x, HEIGHT - 1, 0x00FF00 * (random(0, modes[7].scale) == 0));
    else if (thisColor < 0x002000)
      led.set(x, HEIGHT - 1, 0);
    else
      led.set(x, HEIGHT - 1, thisColor - 0x002000);
  }

  // сдвигаем всё вниз
  for (byte x = 0; x < WIDTH; x++) {
    for (byte y = 0; y < HEIGHT - 1; y++) {
      led.set(x, y, led.get(x, y + 1));
    }
  }
}

// ----------------------------- СВЕТЛЯКИ ------------------------------

#define MAX_SIZE 20
#define MAX_DIST 5
uint16_t cnt = 0;

struct KeyPress {
public:
	byte x, y;
	uint16_t ind;
	KeyPress(byte x, byte y): x(x), y(y) {
		ind = cnt;
	}
	KeyPress() {};
};

KeyPress queue[MAX_SIZE];
byte head = 0;
byte size = 0;
byte cnt1 = 0;

void lightersRoutine() {
	++cnt1;
	if(cnt1 != 3)
		return; cnt1 = 0;
	led.clear();
	++cnt;
	byte bad = 0;
	for(byte i = 0; i < size; ++i) {
		byte ind = (i + head) % MAX_SIZE;
#ifdef DEBUG
		Serial1.println((std::to_string(queue[i].x) + ' ' + std::to_string(queue[i].y) + '\n').c_str());
#endif
		uint16_t age = cnt - queue[ind].ind;
		if(age > MAX_DIST || age < 0) {
			++bad;
			continue;
		}
		uint16_t ages = age * age;
		for(uint16_t x = 0; x < WIDTH; ++x)
			for(uint16_t y = 0; y < HEIGHT; ++y) {
				uint16_t dist = (x - queue[ind].x) * (x - queue[ind].x) + (y - queue[ind].y) * (y - queue[ind].y);
				uint16_t dist1 = dist - ages;
				if(dist1 < 0)
					dist1 = -dist1;
				if(dist1 < age * 2) {
#ifdef DEBUG
					Serial1.println(("Lighting" + std::to_string(x) + ' ' + std::to_string(y) + '\n').c_str());
#endif
					led.set(x, y, mHSV(modes[LIGHTERS_MODE].scale, 255 * (2 * age - dist1) / (2 * age), 255 * (MAX_DIST - age) / MAX_DIST));
				}
		}
	}
	head = (head + bad) % MAX_SIZE;
	size -= bad;
	led.show();
}

void press(byte i, byte j) {
	queue[(head + size) % MAX_SIZE] = KeyPress(i, j);
	if(size != MAX_SIZE)
		size++;
}

#define MAX_AGE 50

void FadeRoutine() {
	++cnt;
	byte bad = 0;
	for(byte i = 0; i < size; ++i) {
		byte ind = (i + head) % MAX_SIZE;
		uint16_t age = cnt - queue[ind].ind;
		if(age > MAX_AGE || age < 0) {
			led.set(queue[ind].x, queue[ind].y, null);
			++bad;
			continue;
		}
		led.set(queue[ind].x, queue[ind].y, mHSV(modes[FADE_MODE].scale, 255, 255 * (MAX_AGE - age) / MAX_AGE));
	}
	head = (head + bad) % MAX_SIZE;
	size -= bad;
	led.show();
}

/*
  void lightersRoutine() {
  if (loadingFlag) {
    loadingFlag = false;
    randomSeed(millis());
    for (byte i = 0; i < LIGHTERS_AM; i++) {
      lightersPos[0][i] = random(0, WIDTH * 10);
      lightersPos[1][i] = random(0, HEIGHT * 10);
      lightersColor[i] = CHSV(random(0, 255), 255, 255);
      speedV[i] = random(5, 10);
      angle[i] = random(0, 360);
      angleSpeed[i] = random(-10, 10);
    }
  }
  FastLED.clear();
  if (++loopCounter > 20) loopCounter = 0;
  for (byte i = 0; i < modes[17].scale; i++) {
    if (loopCounter == 0) {     // меняем скорость каждые 255 отрисовок
      angleSpeed[i] += random(-3, 4);
      angleSpeed[i] = constrain(angleSpeed[i], -15, 15);
    }
    lightersPos[0][i] += speedV[i] * cos(radians(angle[i]));
    lightersPos[1][i] += speedV[i] * sin(radians(angle[i]));
    if (lightersPos[0][i] < 0) lightersPos[0][i] = (WIDTH - 1) * 10;
    if (lightersPos[0][i] >= WIDTH * 10) lightersPos[0][i] = 0;
    if (lightersPos[1][i] < 0) {
      lightersPos[1][i] = 0;
      angle[i] = 360 - angle[i];
    } else {
      angle[i] += angleSpeed[i];
    }
    if (lightersPos[1][i] >= (HEIGHT - 1) * 10) {
      lightersPos[1][i] = (HEIGHT - 1) * 10;
      angle[i] = 360 - angle[i];
    } else {
      angle[i] += angleSpeed[i];
    }
    if (angle[i] > 360) angle[i] = 360 - angle[i];
    if (angle[i] < 0) angle[i] = 360 + angle[i];
    drawPixelXY(lightersPos[0][i] / 10, lightersPos[1][i] / 10, lightersColor[i]);
  }
  }
*/
