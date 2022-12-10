/*
    microLED - ультра-лёгкая библиотека для работы с адресной лентой/матрицей
    Документация: https://alexgyver.ru/microled/
    GitHub: https://github.com/GyverLibs/microLED
    Возможности:
    - Основная фишка: сжатие цвета, код занимает в разы меньше места в SRAM по сравнению с аналогами (FastLED, NeoPixel и др.)
    - Поддержка сжатия цвета: 8, 16 и 24 бита
    - Возможность работать вообще без буфера (с некоторыми ограничениями)
    - Работа с цветом:
    - RGB
    - HSV
    - HEX (WEB цвета)
    - "Цветовое колесо" (1500 или 255 самых ярких оттенков)
    - 16 встроенных цветов
    - Цвет по теплоте
    - Градиенты
    - Возможность чтения сжатого цвета в mHEX 0xRRGGBB и массив RGB
    - Оптимизированный asm вывод
    - Встроенная поддержка работы с адресными матрицами
    - Поддержка чипов: 2811/2812/2813/2815/2818/WS6812/APA102
    - Встроенная tinyLED для работы на ATtiny
    - Совместимость типов данных и инструментов из FastLED
    - Расширенная настройка прерываний
    - Нативная поддержка матриц
    - Сохранение работы millis() (только для AVR)
    - Поддержка SPI лент (программная и аппаратная)
    
    AlexGyver & Egor 'Nich1con' Zaharov, alex@alexgyver.ru
    https://alexgyver.ru/
    MIT License
    
    Версии:
    v1.1
    - Поправлена инициализация
    - Добавлен оранжевый цвет
    
    v2.0
    - Переписан и сильно ускорен алгоритм вывода
    - Добавлено ограничение тока
    
    v2.1
    - Поправлена ошибка с матрицей
    
    v2.2
    - Цвет PINK заменён на MAGENTA
    
    v2.3
    - Добавлена дефайн настройка MICROLED_ALLOW_INTERRUPTS
    - Исправлены мелкие ошибки, улучшена стабильность
    
    v2.4
    - Добавлен ORDER_BGR
    
    v2.5
    - Яркость по CRT гамме
    
    v3.0
    - Добавлены функции и цвета: 
    - Цветовая температура .setKelvin() и дата Kelvin
    - getBlend(позиция, всего, цвет1, цвет2) и getBlend2(позиция, всего, цвет1, цвет2)
    - .fill(от, до)
    - .fillGradient(от, до, цвет1, цвет2)
    - Добавлен шум Перлина (вытащил из FastLED)
    - Добавлены градиенты
    - Полностью переделан и оптимизирован вывод
    - Возможность работать вообще без буфера
    - Настройка ограничения тока для всех типов лент
    - Настраиваемый запрет прерываний
    - Сохранение работы миллиса на время отправки
    - Поддержка лент 2811, 2812, 2813, 2815, 2818
    - Поддержка 4х цветных лент: WS6812
    - Инициализация переделана под шаблон, смотри примеры!
    - Много изменений в названиях, всё переделано и упрощено, читай документацию!
    
    v3.1
    - Поправлены ошибки компиляции для нестандартных ядер Ардуино и Аттини
    - Добавлен класс tinyLED.h для вывода потоком с ATtiny и вообще любых AVR (см. пример)
    - Вырезаны инструменты FastLED (рандом, шум), будем работать напрямую с фастлед
    - Добавлена поддержка совместной работы с библиотекой FastLED и конвертация из её типов!
    - Добавлена поддержка ленты APA102 (а также других SPI), программная и аппаратная SPI
    
    v3.2
    - Чуть оптимизации и исправлений
    
    v3.3
    - Исправлен критический баг с влиянием на другие пины
    
    v3.4
    - Переработан ASM вывод, меньше весит, легче адаптируется под другие частоты / тайминги
    - Добавлена поддержка LGT8F328P с частотой 32/16/8 MHz
    - Переработан поллинг millis()/micros() - прямой вызов прерывания TIMER0_OVF, убран лишний код
    
    v3.5 
    - Исправлена ошибка компиляции в некоторых угодных компилятору случаях
*/
#ifndef microLED_h
#define microLED_h

#ifndef COLOR_DEBTH
#define COLOR_DEBTH 3	// по умолчанию 24 бита
#endif

#include "color_utility.h"

enum M_order {
    // порядок цвета: r-00, g-01, b-10
    ORDER_RGB = 0b000110,
    ORDER_RBG = 0b001001,
    ORDER_BRG = 0b100001,
    ORDER_BGR = 0b100100,
    ORDER_GRB = 0b010010,
    ORDER_GBR = 0b011000,
};

// ========== ПОДКЛЮЧЕНИЕ МАТРИЦЫ ==========
enum M_type {
    ZIGZAG,
    PARALLEL,
};
enum M_connection {
    LEFT_BOTTOM,
    LEFT_TOP,
    RIGHT_TOP,
    RIGHT_BOTTOM,
};
enum M_dir {
    DIR_RIGHT,
    DIR_UP,
    DIR_LEFT,
    DIR_DOWN,
};


// ============================================== КЛАСС ==============================================
// <amount, pin, clock pin, chip, order, cli, mls> ()
// <amount, pin, clock pin, chip, order, cli, mls> (width, height, type, conn, dir)
// количество, пин, чип, порядок, прерывания, миллис
template<int amount, int8_t pin, M_order order>
class microLED {
public:
    /*
        // ЛЕНТА: нет аргументов
        microLED;
        
        // МАТРИЦА: ширина матрицы, высота матрицы, тип матрицы, угол подключения, направление (см. ПОДКЛЮЧЕНИЕ МАТРИЦЫ)
        microLED(uint8_t width, uint8_t height, M_type type, M_connection conn, M_dir dir);
        
        // лента и матрица
        void set(int n, mData color);					// ставим цвет светодиода mData (равносильно leds[n] = color)					
        mData get(int num);								// получить цвет диода в mData (равносильно leds[n])
        void fill(mData color);							// заливка цветом mData
        void fill(int from, int to, mData color);		// заливка цветом mData
        void fillGradient(int from, int to, mData color1, mData color2);	// залить градиентом двух цветов
        void fade(int num, byte val);					// уменьшить яркость
        
        // матрица
        uint16_t getPixNumber(int x, int y);			// получить номер пикселя в ленте по координатам
        void set(int x, int y, mData color);			// ставим цвет пикселя x y в mData
        mData get(int x, int y);						// получить цвет пикселя в mData
        void fade(int x, int y, byte val);				// уменьшить яркость
        void drawBitmap8(int X, int Y, const uint8_t *frame, int width, int height);	// вывод битмапа (битмап 1мерный PROGMEM)
        void drawBitmap16(int X, int Y, const uint16_t *frame, int width, int height);	// вывод битмапа (битмап 1мерный PROGMEM)
        void drawBitmap32(int X, int Y, const uint32_t *frame, int width, int height);	// вывод битмапа (битмап 1мерный PROGMEM)
        
        // общее
        void setMaxCurrent(int ma);						// установить максимальный ток (автокоррекция яркости). 0 - выключено
        void setBrightness(uint8_t newBright);			// яркость 0-255
        void clear();									// очистка
        
        // вывод буфера
        void show();									// вывести весь буфер
        
        // вывод потока
        void begin();									// начать вывод потоком
        void send(mData data);							// отправить один светодиод
        void end();										// закончить вывод потоком
    */
    
    int oneLedMax = 30;
    int oneLedIdle = 660;
    mData leds[amount + 1];

    void init() {
		_pin_mask = (STM_LL_GPIO_PIN(digitalPinToPinName(pin)) >> GPIO_PIN_MASK_POS) & 0x0000FFFFU;
		_port = get_GPIO_Port(STM_PORT(digitalPinToPinName(pin)));
        // oneLedMax = (ток ленты с одним горящим) - (ток выключенной ленты)
        // oneLedIdle = (ток выключенной ленты) / (количество ледов)
    }

    microLED() {
        init();
    }

    microLED(uint8_t width, uint8_t height, M_type type, M_connection conn, M_dir dir) :
    _width(width), _height(height), _matrixConfig( (uint8_t)conn | ((uint8_t)dir << 2) ), _matrixType( (uint8_t)type ) {
        init();
        if (_matrixConfig == 4 || _matrixConfig == 13 || _matrixConfig == 14 || _matrixConfig == 7) _matrixW = height;
        else _matrixW = width;
    }

    void setBrightness(uint8_t newBright) {
        _bright = getCRT(newBright);
    }

    void clear() {
        for (int i = 0; i < amount; i++) leds[i] = 0;
    }

    void fill(mData color) {
        for (int i = 0; i < amount; i++) leds[i] = color;
    }

    void fill(int from, int to, mData color) {
        for (int i = from; i <= to; i++) leds[i] = color;
    }

    void fillGradient(int from, int to, mData color1, mData color2) {
        for (int i = from; i < to; i++) leds[i] = getBlend(i - from, to - from, color1, color2);
    }

    void set(int n, mData color) {
        leds[n] = color;
    }

    mData get(int num) {
        return leds[num];
    }

    void fade(int num, byte val) {
        leds[num] = getFade(leds[num], val);
    }

    // ============================================== МАТРИЦА ==============================================
    uint16_t getPixNumber(int x, int y) {
        return pgm_read_byte(&IND[12 * y + x]);
		int thisX, thisY;
        switch (_matrixConfig) {
        case 0:   thisX = x;          thisY = y;          break;
        case 4:   thisX = y;          thisY = x;          break;
        case 1:   thisX = x;          thisY = (_height - y - 1);  break;
        case 13:  thisX = (_height - y - 1);  thisY = x;          break;
        case 10:  thisX = (_width - x - 1); thisY = (_height - y - 1);  break;
        case 14:  thisX = (_height - y - 1);  thisY = (_width - x - 1); break;
        case 11:  thisX = (_width - x - 1); thisY = y;          break;
        case 7:   thisX = y;          thisY = (_width - x - 1); break;
        }

        if (_matrixType || !(thisY % 2)) return (thisY * _matrixW + thisX); // если чётная строка
        else return (thisY * _matrixW + _matrixW - thisX - 1);        // если нечётная строка
    }

    void set(int x, int y, mData color) {
        //if (x * y >= amount || x < 0 || y < 0 || x >= _width || y >= _height) return;
        leds[getPixNumber(x, y)] = color;
    }

    mData get(int x, int y) {
        return leds[getPixNumber(x, y)];
    }

    void fade(int x, int y, byte val) {
        int pix = getPixNumber(x, y);
        leds[pix] = getFade(leds[pix], val);
    }

    void drawBitmap8(int X, int Y, const uint8_t *frame, int width, int height) {
        for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
        set(x + X, y + Y, pgm_read_byte(&frame[x + (height - 1 - y) * width]));
    }
    void drawBitmap16(int X, int Y, const uint16_t *frame, int width, int height) {
        for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
        set(x + X, y + Y, pgm_read_word(&frame[x + (height - 1 - y) * width]));
    }
    void drawBitmap32(int X, int Y, const uint32_t *frame, int width, int height) {
        for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
        set(x + X, y + Y, pgm_read_dword(&frame[x + (height - 1 - y) * width]));
    }

    // ============================================== УТИЛИТЫ ==============================================
    void setMaxCurrent(int ma) {
        _maxCurrent = ma;
    }

    uint8_t correctBright(uint8_t bright) {
        long sum = 0;
        for (int i = 0; i < amount; i++) {
            sum += fade8R(leds[i], bright);
            sum += fade8G(leds[i], bright);
            sum += fade8B(leds[i], bright);
        }

        sum = ((long)sum >> 8) * oneLedMax / 3;         // текущий "активный" ток ленты
        int idle = (long)oneLedIdle * amount / 1000;      // холостой ток ленты
        if (sum == 0) return bright;
        if ((sum + idle) < _maxCurrent) return bright;      // ограничения нет
        else return ((float)(_maxCurrent - idle) * bright / sum); // пересчёт яркости
    }

    // ============================================== ВЫВОД ==============================================
    void begin() {
        _showBright = _bright;
		noInterrupts();
    }

    void show() {
        begin();
        //if (_maxCurrent != 0 && amount != 0) _showBright = correctBright(_bright);
        for (int i = 0; i < amount; i++) send(leds[i]);
        end();
    }

    void send(mData color) {
        uint8_t data[3];
        // компилятор посчитает сдвиги
        data[(order >> 4) & 0b11] = fade8R(color, _showBright);
        data[(order >> 2) & 0b11] = fade8G(color, _showBright);
        data[order & 0b11] = fade8B(color, _showBright);

        // отправляем RGB и W если есть
        for (uint8_t i = 0; i < 3; i++) sendRaw(data[i]);
    }

	volatile void delayShort(uint32_t num) {
		asm volatile(
			"delay:                    \n\t"
			"subs %[counter], 1;       \n\t"
			"bne delay;                \n\t"
			:
			: [counter] "r" (num)
		);
	}

    void sendRaw(byte data) {
		//LL_GPIO_SetOutputPin(_port, _pin_mask);
		//LL_GPIO_ResetOutputPin(_port, _pin_mask);
		asm volatile(
			"mov r11, 8;\n\t"
			"mainloop:\n\t"
			"str %[pin_mask], [%[port], #16];\n\t"
			"tst %[data], 0x80\n"
			"mov r12, 7\n\t"
			"bne delay1\n\t"
			"mov r12, 3\n\t"
			"delay1:\n\t"
			"subs r12, 1\n\t"
			"bne delay1\n\t"
			"str %[pin_mask], [%[port], #20];\n\t"
			"tst %[data], 0x80\n\t"
			"mov r12, 7\n\t"
			"beq delay2\n\t"
			"mov r12, 3\n\t"
			"delay2:\n\t"
			"subs r12, 1\n\t"
			"bne delay2\n\t"
			"mov %[data], %[data], LSL #1\n\t"
			"subs r11, 1\n\t"
			"bne mainloop\n\t"
			//"beq low\n\t"
			//"str %[pin_mask], [%[port], #20];\n\t"
			:
			: [pin_mask] "r" (_pin_mask), [data] "rw" (data), [port] "r" (_port)
			: "r11", "r12"
		);
		/*
		for(uint8_t mask = 0b10000000; mask; mask >>= 1) {
			//SET_HI
			if (data & mask) {
				LL_GPIO_SetOutputPin(_port, _pin_mask);
				delayShort(10);
				//SET_LO
				LL_GPIO_ResetOutputPin(_port, _pin_mask);
				delayShort(5);
			} else {
				LL_GPIO_SetOutputPin(_port, _pin_mask);
				delayShort(5);
				//SET_LO
				LL_GPIO_ResetOutputPin(_port, _pin_mask);
				delayShort(10);
			}
		}
		*/
	}

    void end() {
		interrupts();
	}

private:
    uint8_t _bright = 50, _showBright = 50;
    const uint8_t _matrixConfig = 0, _matrixType = 0, _width = 0, _height = 0;
    uint8_t _matrixW = 0;
    int _maxCurrent = 0;

    volatile uint32_t _pin_mask;
    volatile GPIO_TypeDef *_port;
};  // класс
#endif
