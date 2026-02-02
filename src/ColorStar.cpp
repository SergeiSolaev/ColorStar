#define FIRWARE_VERSION "1.0.0"

#include "FastLED.h" 

#define DATA_PIN 13      // номер пина для подключения линии данных
#define NUM_LEDS 11      // колличество светодиодов в ленте
#define LED_TYPE WS2811  // тип чипа светодиодов
#define COLOR_ORDER RGB  // порядок цветов
#define BUTTON 4         // пин для кнопки
#define BTN_DEBOUNCE 300 // антидребезг 300 мс

CRGB leds[NUM_LEDS];     // передаём количество светодиодов в массив

int regim = 1;           // номер режима при старте звезды
unsigned int subRegimDuration = 10000; // длительность режимов в цикличном повторе

bool btnState;                  // состояние кнопки
bool trigger1 = false;          // триггер для режимов
bool allRegimIntroDone = false; // для приветственной последовательности зажигания светодиодов в цикличном повторе

unsigned long prevTime = 0;            // таймер для переключения светодиодов
unsigned long prevTimeChangeRegim = 0; // таймер для цикличной смены всех режимов
unsigned long btnTimer;                // таймер для антидребезга

int f1_fadeAmountA = 1;  // для режима Fading_1
int f1_brightnessA = 50; // для режима Fading_1

int f1_fadeAmountB = 1;   // для режима Fading_1
int f1_brightnessB = 200; // для режима Fading_1

int f3_fadeAmountA = 1;  // для режима Fading_3
int f3_brightnessA = 10; // для режима Fading_3

int f3_fadeAmountB = 1;  // для режима Fading_3
int f3_brightnessB = 10; // для режима Fading_3

void handleButton();
void allRegimCycle();
bool allRegimIntro();
void blink1();
void fading1();
void fading2();
void fading3();
void colorPallete();
void colorPalleteStrobWhite();

void setup()
{
  pinMode(BUTTON, INPUT);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS); // показываем бибилиотеке fastLED параметры диодов и подключеня
}

void loop()
{

  handleButton();

  if (btnState)
  {
    btnState = false;
    regim++;
    if (regim > 7)
    {
      regim = 1;
    }
  }

  static uint8_t prevRegim = 0;
  if (prevRegim != regim)
  {
    if (regim == 1)
      allRegimIntroDone = false;
    prevRegim = regim;
  }

  switch (regim)
  {
  case 1:
    allRegimCycle();
    break;
  case 2:
    blink1();
    break;
  case 3:
    fading1();
    break;
  case 4:
    fading2();
    break;
  case 5:
    fading3();
    break;
  case 6:
    colorPallete();
    break;
  case 7:
    colorPalleteStrobWhite();
    break;
  default:
    break;
  }
}

// цикличная смена всех режимов
void allRegimCycle()
{
  if (!allRegimIntroDone)
  {
    allRegimIntroDone = allRegimIntro();
    return;
  }

  static int subRegim = 1;
  if (millis() - prevTimeChangeRegim > subRegimDuration)
  {
    subRegim++;
    prevTimeChangeRegim = millis();
    if (subRegim > 6)
    {
      subRegim = 1;
    }
  }

  switch (subRegim)
  {
  case 1:
    blink1();
    break;
  case 2:
    fading1();
    break;
  case 3:
    fading2();
    break;
  case 4:
    fading3();
    break;
  case 5:
    colorPallete();
    break;
  case 6:
    colorPalleteStrobWhite();
    break;
  default:
    break;
  }
}

// обработчик кнопки с антидребезгом
void handleButton()
{
  static bool lastButtonState = HIGH; // Предыдущее состояние кнопки
  bool currentButtonState = digitalRead(BUTTON);

  // детектируем нажатие (переход с HIGH на LOW)
  if (currentButtonState == LOW && lastButtonState == HIGH &&
      millis() - btnTimer > BTN_DEBOUNCE)
  {
    btnState = true; // Флаг нажатия
    btnTimer = millis();
  }
  else if (currentButtonState == HIGH)
  {
    lastButtonState = HIGH; // Кнопка отпущена
  }

  // сохраняем текущее состояние для следующего вызова
  lastButtonState = currentButtonState;
}

// индикация включения авторежима
bool allRegimIntro()
{
  static int index = NUM_LEDS - 1;
  static unsigned long timer = 0;

  if (millis() - timer < 120)
    return false;

  FastLED.clear();

  for (int i = NUM_LEDS - 1; i >= index; i--)
  {
    uint8_t hue = map(i, 0, NUM_LEDS - 1, 0, 255);
    leds[i] = CHSV(hue, 255, 150);
  }

  FastLED.show();
  timer = millis();

  index--;

  if (index < 0)
  {
    index = NUM_LEDS - 1;
    return true;
  }

  return false;
}

// смена цветов «красный, зелёный – синий, жёлтый» два раза в секунду
void blink1()
{
  if (millis() - prevTime >= 500) // время переключения
  {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      uint8_t pos = i % 4;

      if (!trigger1)
      {
        // состояние A
        if (pos == 0)
          leds[i] = CRGB::Green;
        else if (pos == 1)
          leds[i] = CRGB::Black;
        else if (pos == 2)
          leds[i] = CRGB::Red;
        else
          leds[i] = CRGB::Black;
      }
      else
      {
        // состояние B
        if (pos == 0)
          leds[i] = CRGB::Black;
        else if (pos == 1)
          leds[i] = CRGB::Yellow;
        else if (pos == 2)
          leds[i] = CRGB::Black;
        else
          leds[i] = CRGB::Blue;
      }
    }

    trigger1 = !trigger1;
    prevTime = millis();
    FastLED.show();
  }
}

// плавная смена цветов «красный, зелёный – синий, жёлтый»

void fading1()
{
  if (millis() - prevTime >= 7.5) // время переключения
  {
    if (trigger1)
    {
      // чётные светодиоды
      for (uint8_t i = 0; i < NUM_LEDS; i += 2)
      {
        uint8_t hue = (i % 4 == 0) ? 85 : 255;
        leds[i] = CHSV(hue, 255, f1_brightnessA);
      }

      f1_brightnessA += f1_fadeAmountA;
      if (f1_brightnessA == 50 || f1_brightnessA == 200)
      {
        f1_fadeAmountA = -f1_fadeAmountA;
      }
    }
    else
    {
      // нечётные светодиоды
      for (uint8_t i = 1; i < NUM_LEDS; i += 2)
      {
        uint8_t hue = (i % 4 == 1) ? 42 : 155;
        leds[i] = CHSV(hue, 255, f1_brightnessB);
      }

      f1_brightnessB -= f1_fadeAmountB;
      if (f1_brightnessB == 200 || f1_brightnessB == 50)
      {
        f1_fadeAmountB = -f1_fadeAmountB;
      }
    }

    prevTime = millis();
    FastLED.show();
    trigger1 = !trigger1;
  }
}

// плавное затухание и разгорание всех диодов одновременно
void fading2()
{
  static const CRGB colors[] =
      {
          CRGB::Green,
          CRGB::Yellow,
          CRGB::Red,
          CRGB::Blue};

  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = colors[i % 4];
  }

  FastLED.setBrightness(f3_brightnessA);
  f3_brightnessA += f3_fadeAmountA;

  if (f3_brightnessA <= 10 || f3_brightnessA >= 150)
    f3_fadeAmountA = -f3_fadeAmountA;

  FastLED.show();
  FastLED.delay(25);
}

// плавное затухание и разгорание всех светодиодов одновременно со сменой цвета
void fading3()
{
  static uint8_t starthue = 0;
  fill_rainbow(leds, NUM_LEDS, --starthue, 20);
  FastLED.setBrightness(f3_brightnessB);
  f3_brightnessB = f3_brightnessB + f3_fadeAmountB;

  if (f3_brightnessB == 10 || f3_brightnessB == 150)
  {
    f3_fadeAmountB = -f3_fadeAmountB;
  }

  FastLED.show();
  FastLED.delay(25);
}

// плавная смена цветов по всей цветовой палитре
void colorPallete()
{
  static uint8_t starthue = 0;
  fill_rainbow(leds, NUM_LEDS, --starthue, 20);
  FastLED.setBrightness(150);
  FastLED.show();
  FastLED.delay(25);
}

// плавная смена цветов по всей цветовой палитре с белыми вспышками
void colorPalleteStrobWhite()
{
  static uint8_t starthue = 0;
  fill_rainbow(leds, NUM_LEDS, --starthue, 20);
  leds[random16(NUM_LEDS)] += CRGB::White;
  FastLED.show();
  FastLED.delay(25);
}
