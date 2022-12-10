// служебные функции

// залить все
void fillAll(mData color) {
  led.fill(color);
}

// функция получения цвета пикселя по его номеру
uint32_t getPixColor(int i) {
  mData color = led.get(pgm_read_byte(&IND[i]));
  return (((uint32_t)color.r << 16) | ((long)color.g << 8 ) | (long)color.b);
}

uint16_t getPixelNumber(int8_t x, int8_t y) {
  return led.getPixNumber(x, y);
}
