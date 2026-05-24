/*
14 - Круг (Circle)
12 - Треугольник (Triangle)
15 - Крест
4 - Квадрат (Square)

Кнопки

0 - Вкл/Выкл - btnOn
2 - Назад - btnPrev
5 - Веперд - btnNext
13  - Режим btnMode

Внопка Вкл/Выкл - включает светильник
В обычном режиме, кнопки назад и вперед регулируют яркость.
Кнопка Режим,позволяет переключатся между режимами
*/


// #define EB_NO_FOR           // отключить поддержку pressFor/holdFor/stepFor и счётчик степов (экономит 2 байта оперативки)
// #define EB_NO_CALLBACK      // отключить обработчик событий attach (экономит 2 байта оперативки)
// #define EB_NO_COUNTER       // отключить счётчик энкодера (экономит 4 байта оперативки)
// #define EB_NO_BUFFER        // отключить буферизацию энкодера (экономит 1 байт оперативки)

// #define EB_DEB_TIME 50      // таймаут гашения дребезга кнопки (кнопка)
// #define EB_CLICK_TIME 500   // таймаут ожидания кликов (кнопка)
// #define EB_HOLD_TIME 600    // таймаут удержания (кнопка)
#define EB_STEP_TIME 50    // таймаут импульсного удержания (кнопка)
// #define EB_FAST_TIME 30     // таймаут быстрого поворота (энкодер)

#include <EncButton.h>                      //Библиотека для кнопки
#include <EEManager.h>                      //Библитека памяти

#define MAX_BRIGHTNESS 255 //Максимальная яркость 0-255
#define MIN_BRIGHTNESS 5  //Минимальная яркость 255-0
#define MIN_BRIGHTNESS_FADE_MODE 0  //Минимальная яркость в режиме плавного затухания 255-0
#define STEP_AUTO_BRIGHTNESS 5  //Шаг регулировки яркости по удержанию кнопки
#define STEP_MANUAL_BRIGHTNESS 10  //Шаг регулировки яркости по 1 нажатию
#define FADE_TIME_MODE_1 20 // Время увеличения и уменьшения яркости в мс (Default 20 мс)
#define FADE_TIME_MODE_2 5 // Время увеличения и уменьшения яркости в мс (Default 20 мс)
#define HOLD_ON_TIME_MODE_2 5000UL // Пауза после полного включения фигур (5 секунд)
#define HOLD_OFF_TIME_MODE_2 5000UL // Пауза после полного выключения фигур (5 секунд)
#define RANDOM_FADE_INTERVAL_MODE_4 8UL
#define RANDOM_DELAY_MIN_MODE_4 150UL
#define RANDOM_DELAY_MAX_MODE_4 1200UL
#define PULSE_INTERVAL_MODE_5_MIN 5UL
#define PULSE_INTERVAL_MODE_5_MAX 20UL
#define LIGHTNING_FADE_INTERVAL 6UL
#define LIGHTNING_FADE_STEP_DOWN_MODE_7_8 1
#define POWER_OFF_FADE_INTERVAL 10UL
#define AUTO_MODE_INTERVAL 60000UL

#define LED_PIN_TRIANGLE 12     //  Пин - Треугольник
#define LED_PIN_CIRCLE 14       //  Пин - Круг
#define LED_PIN_X 15            //  Пин - Икс
#define LED_PIN_SQUARE 4        //  Пин - Квадрат
#define BUTTON_PIN_ON 0         //  Пин - Кнопка Вкл/Выкл
#define BUTTON_PIN_PREV 2       //  Пин - Кнопка Назад
#define BUTTON_PIN_NEXT 13      //  Пин - Кнопка Вперед
#define BUTTON_PIN_MODE 5       //  Пин - Кнопка Режим

Button btnOn(BUTTON_PIN_ON, INPUT_PULLUP); // Вкл/Выкл - btnOn
Button btnPrev(BUTTON_PIN_PREV, INPUT_PULLUP); // Назад - btnPrev
Button btnNext(BUTTON_PIN_NEXT, INPUT_PULLUP); // Веперд - btnNext
Button btnMode(BUTTON_PIN_MODE, INPUT_PULLUP);  // Режим btnMode

const uint8_t ledPin[] = {LED_PIN_TRIANGLE, LED_PIN_CIRCLE, LED_PIN_X, LED_PIN_SQUARE};
const uint8_t countLedsPin = sizeof(ledPin) / sizeof(ledPin[0]);

bool powerOn = false;
bool dir = true;
bool directionBrightness = true;
bool flag = true;

uint8_t globalBrightness = 100;
uint8_t valueBrightness = 0;
uint8_t counterBrightness = 0;
uint8_t currentMode = 0;
uint8_t oldMode = 0;
uint8_t val = 0;
uint8_t k = 0;
uint8_t mode2Step = 0;
uint8_t randomLedIndex = 0;
bool randomLedDirectionUp = true;
uint8_t randomLedBrightness[4] = {0, 0, 0, 0};
uint8_t pulseBrightness[4] = {0, 0, 0, 0};
uint8_t shuffledLedOrder[4] = {0, 1, 2, 3};
uint8_t shuffledLedOrderIndex = 0;
bool pulseDirectionUp[4] = {true, true, true, true};
uint32_t pulseLedTimer[4] = {0, 0, 0, 0};
uint16_t pulseLedInterval[4] = {0, 0, 0, 0};
uint8_t mode6Brightness[4] = {0, 0, 0, 0};
bool mode6DirectionUp[4] = {true, true, true, true};
uint32_t mode6Timer[4] = {0, 0, 0, 0};
uint16_t mode6Interval[4] = {0, 0, 0, 0};

uint8_t mode8Brightness[4] = {0, 0, 0, 0};
bool mode8DirectionUp[4] = {true, true, true, true};
bool mode8Active[4] = {false, false, false, false};
uint8_t autoModeCurrent = 0;
uint32_t autoModeTimer = 0;
uint8_t powerTransitionState = 0; // 0 - нет перехода, 1 - мерцание при включении, 2 - затухание при выключении
uint8_t startupFlickerStep = 0;
uint8_t shutdownFadeBrightness = 0;
uint32_t randomModeDelayStart = 0;
uint32_t previousMillis, currentMillis, timerMode, randomModeDelay = 0;

struct Data {
  bool powerOn = false;
  uint8_t globalBrightness = 100;
  uint8_t currentMode = 0;
};

Data data;  // переменная, с которой мы работаем в программе

EEManager memory(data, 10000); // передаём нашу переменную (фактически её адрес)
void initModeState(uint8_t mode);
bool handlePowerTransition();
void runCurrentMode(uint8_t modeToRun);

void setup() {
  Serial.begin(9600);
  for(int i = 0; i < countLedsPin; i++){
    pinMode(ledPin[i], OUTPUT);
  }

  EEPROM.begin(10);

  /*
    Запускаем менеджер, передаём:
    - Стартовый адрес в памяти для записи даты
    - Ключ хранения (0.. 255) или символ
  */
  byte stat = memory.begin(0, 'b');

  /*
    Коды возврата:
    0 - ключ совпал, данные прочитаны из епром
    1 - ключ не совпал (первый запуск), данные записаны в епром
    2 - ошибка, в епроме не хватает места
  */
  Serial.println(stat);

  powerOn = data.powerOn;
  globalBrightness = data.globalBrightness;
  currentMode = data.currentMode;
  oldMode = currentMode;
  randomSeed(micros());
  initModeState(currentMode);
}

void loop() {
  // в лупе вызываем tick(), в нём по таймеру произойдёт обновление
  // функция вернёт true, когда это случится
  if (memory.tick()) Serial.println("Updated!");
  // запоминаем время
  currentMillis = millis();   // текущее время в миллисекундах

  btnOn.tick();
  btnNext.tick();
  btnPrev.tick();
  btnMode.tick();

  //Если нажали кнопку включения, то меняем значение powerOn на противоположное
  if (btnOn.click()) {
    if (!powerOn) {
      startupFlickerStep = 0;
      powerTransitionState = 1;
      timerMode = millis();
    } else {
      shutdownFadeBrightness = globalBrightness;
      powerTransitionState = 2;
      timerMode = millis();
    }
    powerOn = !powerOn;
    Serial.println("btnOn - click");
    data.powerOn = powerOn;
    memory.update();
  }

  //Если нажали кнопку увеличения яркости, то прибавляем яркость на количество STEP_BRIGHTNESS
  if (btnNext.click()) {
    if(globalBrightness >= MAX_BRIGHTNESS){ //Если яркость уже больше или равна максимальной
      globalBrightness = MAX_BRIGHTNESS;
      Serial.print("Brightness = ");
      Serial.println(globalBrightness);
      showMinMaxBrightness();
    }else{ //Если яркость меньше максимальной
      globalBrightness = min<uint8_t>(MAX_BRIGHTNESS, globalBrightness + STEP_MANUAL_BRIGHTNESS);
      Serial.print("Brightness = ");
      Serial.println(globalBrightness);
    }
    Serial.println("btnNext - click");
    data.globalBrightness = globalBrightness;
    memory.update();
  }

  //Если удерживаем нажатой кнопку увеличения яркости
  if (btnNext.step()) {
    if (globalBrightness < MAX_BRIGHTNESS){ //Если яркость меньше или равна максимальной
      globalBrightness = min<uint8_t>(MAX_BRIGHTNESS, globalBrightness + STEP_AUTO_BRIGHTNESS);
      Serial.print("Brightness = ");
      Serial.println(globalBrightness);
    }else{
      Serial.print("Brightness = ");
      Serial.println(globalBrightness);
      showMinMaxBrightness();
    }
    Serial.println("btnNext - step");
    data.globalBrightness = globalBrightness;
    memory.update();
  }

  //Если нажали кнопку уменьшения яркости, то убавляем яркость на количество STEP_BRIGHTNESS
  if (btnPrev.click()) {
    if(globalBrightness <= MIN_BRIGHTNESS){ // Если яркость уже меньше или равна минимальной
      globalBrightness = MIN_BRIGHTNESS;
      Serial.print("Brightness = ");
      Serial.println(globalBrightness);
      showMinMaxBrightness();
    }else{ //Если яркость больше минимальной
      globalBrightness = max<uint8_t>(MIN_BRIGHTNESS, globalBrightness - STEP_MANUAL_BRIGHTNESS);
      Serial.print("Brightness = ");
      Serial.println(globalBrightness);
    } 
    Serial.println("btnPrev - click");
    data.globalBrightness = globalBrightness;
    memory.update();
  }

  //Если удерживаем нажатой кнопку уменьшения яркости
  if (btnPrev.step()) {
    if (globalBrightness > MIN_BRIGHTNESS){ //Если яркость больше или равна минимальной
      globalBrightness = max<uint8_t>(MIN_BRIGHTNESS, globalBrightness - STEP_AUTO_BRIGHTNESS);
      Serial.print("Brightness = ");
      Serial.println(globalBrightness);
    }else{
      Serial.print("Brightness = ");
      Serial.println(globalBrightness);
      showMinMaxBrightness();
    }
    Serial.println("btnPrev - step");
    data.globalBrightness = globalBrightness;
    memory.update();
  }

/*******************************************Кнопка переключения режимов****************************************************/
  if (btnMode.click()) 
  {
    //oldMode = currentMode;
    currentMode++;
    if (currentMode > 11) currentMode = 0;
    initModeState(currentMode);
    Serial.println("btnMode - click");
    data.currentMode = currentMode;
    memory.update();
  }
/********************************************Основной цикл работы программы**********************************************/
  if (powerTransitionState != 0 && handlePowerTransition()) return;

  if (powerOn){
    if (currentMode == 11 && millis() - autoModeTimer >= AUTO_MODE_INTERVAL) {
      autoModeCurrent = random(0, 11);
      initModeState(autoModeCurrent);
      autoModeTimer = millis();
    }
    uint8_t modeToRun = (currentMode == 11) ? autoModeCurrent : currentMode;
    runCurrentMode(modeToRun);
  }else{
    setLedsOff(); //Выключаем все диоды
  }
}

void runCurrentMode(uint8_t modeToRun) {
  switch(modeToRun){
      case 0:
        setLedsBrightness(globalBrightness, globalBrightness, globalBrightness, globalBrightness);
          break;
      case 1:
        if (globalBrightness > MIN_BRIGHTNESS && globalBrightness <=50 && millis() - timerMode >= 70
        || globalBrightness > 50 && globalBrightness <= 100 && millis() - timerMode >= 60
        || globalBrightness > 100 && globalBrightness <= 150 && millis() - timerMode >= 50
        || globalBrightness > 150 && globalBrightness <= 200 && millis() - timerMode >= 40
        || globalBrightness > 200 && globalBrightness <= MAX_BRIGHTNESS && millis() - timerMode >= FADE_TIME_MODE_1) {
          timerMode = millis();
          if (directionBrightness) valueBrightness++; // увеличиваем яркость
          else valueBrightness--;     // уменьшаем
          if(valueBrightness >= globalBrightness) valueBrightness = globalBrightness;
          if (valueBrightness <= MIN_BRIGHTNESS_FADE_MODE) valueBrightness = MIN_BRIGHTNESS_FADE_MODE;
          if (valueBrightness >= globalBrightness || valueBrightness <= MIN_BRIGHTNESS_FADE_MODE) directionBrightness = !directionBrightness; // разворачиваем
        }
        setLedsBrightness(valueBrightness, valueBrightness, valueBrightness, valueBrightness);
          break;
      case 2:
        if (millis() - timerMode >= FADE_TIME_MODE_2){
          if (mode2Step == 0) {
            timerMode = millis();  // Плавно включаем фигуры: Треугольник -> Круг -> Крест -> Квадрат
            counterBrightness++;
            analogWrite(ledPin[val], crt3_8(counterBrightness));
            if (counterBrightness == 255) {
              counterBrightness = 0;
              val++;
              if (val >= countLedsPin) {
                mode2Step = 1;
                timerMode = millis();
                val = countLedsPin - 1;
              }
            }
          } else if (mode2Step == 2) {  // Плавно выключаем в обратном порядке: Квадрат -> Крест -> Круг -> Треугольник
            timerMode = millis();
            if (counterBrightness < 255) {
              counterBrightness++;
              analogWrite(ledPin[val], crt3_8(255 - counterBrightness));
            }
            if (counterBrightness == 255) {
              counterBrightness = 0;
              if (val == 0) {
                mode2Step = 3;
                timerMode = millis();
              } else {
                val--;
              }
            }
          }
        }
        if (mode2Step == 1 && millis() - timerMode >= HOLD_ON_TIME_MODE_2) {  // Пауза 5 сек во включенном состоянии
          mode2Step = 2;
          val = countLedsPin - 1;
          counterBrightness = 0;
        } else if (mode2Step == 3 && millis() - timerMode >= HOLD_OFF_TIME_MODE_2) {  // Пауза 5 сек в выключенном состоянии
          mode2Step = 0;
          val = 0;
          counterBrightness = 0;
        }
          break;
      case 3:
        if (millis() - timerMode >= FADE_TIME_MODE_2){
          if (mode2Step == 0) {
            timerMode = millis();  // Плавно включаем фигуры: Квадрат -> Крест -> Круг -> Треугольник
            counterBrightness++;
            analogWrite(ledPin[val], crt3_8(counterBrightness));
            if (counterBrightness == 255) {
              counterBrightness = 0;
              if (val == 0) {
                mode2Step = 1;
                timerMode = millis();
                val = 0;
              } else {
                val--;
              }
            }
          } else if (mode2Step == 2) {  // Плавно выключаем в обратном порядке: Треугольник -> Круг -> Крест -> Квадрат
            timerMode = millis();
            if (counterBrightness < 255) {
              counterBrightness++;
              analogWrite(ledPin[val], crt3_8(255 - counterBrightness));
            }
            if (counterBrightness == 255) {
              counterBrightness = 0;
              if (val >= countLedsPin - 1) {
                mode2Step = 3;
                timerMode = millis();
              } else {
                val++;
              }
            }
          }
        }
        if (mode2Step == 1 && millis() - timerMode >= HOLD_ON_TIME_MODE_2) {  // Пауза 5 сек во включенном состоянии
          mode2Step = 2;
          val = 0;
          counterBrightness = 0;
        } else if (mode2Step == 3 && millis() - timerMode >= HOLD_OFF_TIME_MODE_2) {  // Пауза 5 сек в выключенном состоянии
          mode2Step = 0;
          val = countLedsPin - 1;
          counterBrightness = 0;
        }
          break;
      case 4:
        if (millis() - timerMode >= RANDOM_FADE_INTERVAL_MODE_4) {
          timerMode = millis();

          if (randomLedDirectionUp) {
            if (randomLedBrightness[randomLedIndex] < globalBrightness) {
              randomLedBrightness[randomLedIndex]++;
            } else {
              randomLedDirectionUp = false;
            }
          } else {
            if (randomLedBrightness[randomLedIndex] > 0) {
              randomLedBrightness[randomLedIndex]--;
            } else if (randomModeDelay == 0) {
              randomModeDelay = random(RANDOM_DELAY_MIN_MODE_4, RANDOM_DELAY_MAX_MODE_4 + 1);
              randomModeDelayStart = millis();
            }
          }
        }

        if (!randomLedDirectionUp && randomLedBrightness[randomLedIndex] == 0 && randomModeDelay > 0) {
          if (millis() - randomModeDelayStart >= randomModeDelay) {
            randomModeDelay = 0;
            randomLedIndex = random(countLedsPin);
            randomLedDirectionUp = true;
            timerMode = millis();
          }
        }

        for (uint8_t i = 0; i < countLedsPin; i++) {
          analogWrite(ledPin[i], crt3_8(randomLedBrightness[i]));
        }
          break;
      case 5:
        for (uint8_t i = 0; i < countLedsPin; i++) {
          if (millis() - pulseLedTimer[i] >= pulseLedInterval[i]) {
            pulseLedTimer[i] = millis();
            if (pulseDirectionUp[i]) {
              if (pulseBrightness[i] < globalBrightness) pulseBrightness[i]++;
              else pulseDirectionUp[i] = false;
            } else {
              if (pulseBrightness[i] > 0) pulseBrightness[i]--;
              else pulseDirectionUp[i] = true;
            }

            if (pulseBrightness[i] == 0 || pulseBrightness[i] == globalBrightness) {
              pulseLedInterval[i] = random(PULSE_INTERVAL_MODE_5_MIN, PULSE_INTERVAL_MODE_5_MAX + 1);
            }
          }
        }

        setLedsBrightness(
          crt3_8(pulseBrightness[0]),
          crt3_8(pulseBrightness[1]),
          crt3_8(pulseBrightness[2]),
          crt3_8(pulseBrightness[3])
        );
          break;
      case 6:
        for (uint8_t i = 0; i < countLedsPin; i++) {
          if (millis() - mode6Timer[i] >= mode6Interval[i]) {
            mode6Timer[i] = millis();
            if (mode6DirectionUp[i]) {
              if (mode6Brightness[i] < globalBrightness) mode6Brightness[i]++;
              else mode6DirectionUp[i] = false;
            } else {
              if (mode6Brightness[i] > 0) mode6Brightness[i]--;
              else mode6DirectionUp[i] = true;
            }
            if (mode6Brightness[i] == 0 || mode6Brightness[i] == globalBrightness) {
              mode6Interval[i] = random(PULSE_INTERVAL_MODE_5_MIN, PULSE_INTERVAL_MODE_5_MAX + 1);
            }
          }
          analogWrite(ledPin[i], crt3_8(mode6Brightness[i]));
        }
          break;
      case 7:
        if (millis() - timerMode >= LIGHTNING_FADE_INTERVAL) {
          timerMode = millis();
          if (randomLedBrightness[randomLedIndex] > LIGHTNING_FADE_STEP_DOWN_MODE_7_8) {
            randomLedBrightness[randomLedIndex] -= LIGHTNING_FADE_STEP_DOWN_MODE_7_8;
          } else {
            randomLedBrightness[randomLedIndex] = 0;
            randomLedIndex = random(countLedsPin);
            randomLedBrightness[randomLedIndex] = globalBrightness;
          }
        }
        for (uint8_t i = 0; i < countLedsPin; i++) analogWrite(ledPin[i], crt3_8(randomLedBrightness[i]));
          break;
      case 8:
        if (millis() - timerMode >= LIGHTNING_FADE_INTERVAL) {
          timerMode = millis();

          uint8_t activeCount = 0;
          for (uint8_t i = 0; i < countLedsPin; i++) if (mode8Active[i]) activeCount++;

          if (activeCount == 0) {
            uint8_t targetCount = random(1, countLedsPin + 1);
            while (targetCount > 0) {
              uint8_t idx = random(countLedsPin);
              if (!mode8Active[idx]) {
                mode8Active[idx] = true;
                mode8Brightness[idx] = globalBrightness;
                targetCount--;
              }
            }
          }

          for (uint8_t i = 0; i < countLedsPin; i++) {
            if (mode8Active[i]) {
              if (mode8Brightness[i] > LIGHTNING_FADE_STEP_DOWN_MODE_7_8) {
                mode8Brightness[i] -= LIGHTNING_FADE_STEP_DOWN_MODE_7_8;
              } else {
                mode8Brightness[i] = 0;
                mode8Active[i] = false;
              }
            }
            analogWrite(ledPin[i], crt3_8(mode8Brightness[i]));
          }
        }
          break;
      case 9:
        if (millis() - timerMode >= FADE_TIME_MODE_2){
          if (mode2Step == 0) {
            timerMode = millis();
            counterBrightness++;
            analogWrite(ledPin[val], crt3_8(counterBrightness));
            if (counterBrightness == 255) {
              counterBrightness = 0;
              val++;
              if (val >= countLedsPin) {
                mode2Step = 1;
                timerMode = millis();
                val = 0;
              }
            }
          } else if (mode2Step == 2) {
            timerMode = millis();
            if (counterBrightness < 255) {
              counterBrightness++;
              analogWrite(ledPin[val], crt3_8(255 - counterBrightness));
            }
            if (counterBrightness == 255) {
              counterBrightness = 0;
              val++;
              if (val >= countLedsPin) {
                mode2Step = 3;
                timerMode = millis();
              }
            }
          }
        }
        if (mode2Step == 1 && millis() - timerMode >= HOLD_ON_TIME_MODE_2) {
          mode2Step = 2;
          val = 0;
          counterBrightness = 0;
        } else if (mode2Step == 3 && millis() - timerMode >= HOLD_OFF_TIME_MODE_2) {
          mode2Step = 0;
          val = 0;
          counterBrightness = 0;
        }
          break;
      case 10:
        if (millis() - timerMode >= FADE_TIME_MODE_2){
          if (mode2Step == 0) {
            timerMode = millis();
            counterBrightness++;
            analogWrite(ledPin[val], crt3_8(counterBrightness));
            if (counterBrightness == 255) {
              counterBrightness = 0;
              if (val == 0) {
                mode2Step = 1;
                timerMode = millis();
                val = countLedsPin - 1;
              } else {
                val--;
              }
            }
          } else if (mode2Step == 2) {
            timerMode = millis();
            if (counterBrightness < 255) {
              counterBrightness++;
              analogWrite(ledPin[val], crt3_8(255 - counterBrightness));
            }
            if (counterBrightness == 255) {
              counterBrightness = 0;
              if (val == 0) {
                mode2Step = 3;
                timerMode = millis();
              } else {
                val--;
              }
            }
          }
        }
        if (mode2Step == 1 && millis() - timerMode >= HOLD_ON_TIME_MODE_2) {
          mode2Step = 2;
          val = countLedsPin - 1;
          counterBrightness = 0;
        } else if (mode2Step == 3 && millis() - timerMode >= HOLD_OFF_TIME_MODE_2) {
          mode2Step = 0;
          val = countLedsPin - 1;
          counterBrightness = 0;
        }
          break;
      case 100:
        // если светодиод включен и светится больше чем надо
        if(millis() - timerMode >= 50 && !flag)
        {
          flag = true;                   // включаем
          timerMode = millis();   // запоминаем момент времени
          setLedsBrightness(globalBrightness, globalBrightness, globalBrightness, globalBrightness); //Устанавливаем глобальную яркость всем диодам
          k++; // Счиатем
          if (k > 2 ) currentMode = oldMode; // Если мигнули 3 раза, возвращаемся в старый режим.
          Serial.print("k = ");
          Serial.println(k);
          Serial.print("currentMode = ");
          Serial.println(currentMode);
        }
        else if (flag && (millis() - timerMode >= 50) && currentMode != oldMode)    //Проверяем флаг, время и возврат в старый режим
        {
          flag = false;                   // выключаем
          timerMode = millis();   // запоминаем момент времени
          setLedsOff(); //Выключаем все диоды
        }
      default:
          break;
    }
}

void initModeState(uint8_t mode) {
  setLedsOff();
  timerMode = millis();

  switch(mode) {
    case 1:
      directionBrightness = true;
      valueBrightness = 0;
      break;
    case 2:
    case 9:
      counterBrightness = 0;
      val = 0;
      mode2Step = 0;
      break;
    case 3:
    case 10:
      counterBrightness = 0;
      val = countLedsPin - 1;
      mode2Step = 0;
      break;
    case 4:
      for (uint8_t i = 0; i < countLedsPin; i++) randomLedBrightness[i] = 0;
      randomLedIndex = random(countLedsPin);
      randomLedDirectionUp = true;
      randomModeDelay = 0;
      randomModeDelayStart = 0;
      break;
    case 5:
      for (uint8_t i = 0; i < countLedsPin; i++) {
        pulseBrightness[i] = random(globalBrightness + 1);
        pulseDirectionUp[i] = random(2);
        pulseLedTimer[i] = millis();
        pulseLedInterval[i] = random(PULSE_INTERVAL_MODE_5_MIN, PULSE_INTERVAL_MODE_5_MAX + 1);
      }
      break;
    case 6:
      for (uint8_t i = 0; i < countLedsPin; i++) {
        mode6Brightness[i] = random(globalBrightness + 1);
        mode6DirectionUp[i] = random(2);
        mode6Timer[i] = millis();
        mode6Interval[i] = random(PULSE_INTERVAL_MODE_5_MIN, PULSE_INTERVAL_MODE_5_MAX + 1);
      }
      break;
    case 7:
      for (uint8_t i = 0; i < countLedsPin; i++) randomLedBrightness[i] = 0;
      randomLedIndex = random(countLedsPin);
      randomLedBrightness[randomLedIndex] = globalBrightness;
      randomLedDirectionUp = false;
      break;
    case 8:
      for (uint8_t i = 0; i < countLedsPin; i++) {
        mode8Brightness[i] = 0;
        mode8DirectionUp[i] = false;
        mode8Active[i] = false;
      }
      break;
    case 100:
    case 11:
      autoModeCurrent = random(0, 11);
      autoModeTimer = millis();
      initModeState(autoModeCurrent);
      break;
    default:
      k = 0;
      flag = true;
      break;
  }

  Serial.print("Mode = ");
  Serial.println(mode);
}

/* ==========================================Выключить все светодиоды===============================*/

void setLedsOff() {
  for(int i = 0; i < countLedsPin; i++){
    analogWrite(ledPin[i], 0);
  }
}
/* ==========================================Установить светодиодам заданную яркость===============================*/
void setLedsBrightness(uint8_t ledBrightnessTriangle, uint8_t ledBrightnessCircle, uint8_t ledBrightnessX, uint8_t ledBrightnessSquare){
  analogWrite(LED_PIN_TRIANGLE, ledBrightnessTriangle);
  analogWrite(LED_PIN_CIRCLE, ledBrightnessCircle);
  analogWrite(LED_PIN_X, ledBrightnessX);
  analogWrite(LED_PIN_SQUARE, ledBrightnessSquare);
}

void showMinMaxBrightness(){
  if (currentMode != 100){
    oldMode = currentMode;
    setLedsOff();
    timerMode = millis();
    k = 0;
    flag = true;
    currentMode = 100;
  }else{
    currentMode = oldMode;
  }
}


void shuffleLedOrder() {
  for (uint8_t i = 0; i < countLedsPin; i++) shuffledLedOrder[i] = i;
  for (int8_t i = countLedsPin - 1; i > 0; i--) {
    uint8_t j = random(i + 1);
    uint8_t tmp = shuffledLedOrder[i];
    shuffledLedOrder[i] = shuffledLedOrder[j];
    shuffledLedOrder[j] = tmp;
  }
}

// кубическая гамма для 8 бит
uint8_t crt3_8(uint8_t val) {
  return ((uint32_t)(val + 1) * (val + 1) * val) >> 16;
}

bool handlePowerTransition() {
  if (powerTransitionState == 1) {
    if (millis() - timerMode >= 70) {
      timerMode = millis();
      static const uint8_t flickerPattern[] = {0, 220, 30, 255, 0, 180, 70, 255};
      uint8_t level = min<uint8_t>(globalBrightness, flickerPattern[startupFlickerStep]);
      setLedsBrightness(crt3_8(level), crt3_8(level), crt3_8(level), crt3_8(level));
      startupFlickerStep++;
      if (startupFlickerStep >= sizeof(flickerPattern)) {
        powerTransitionState = 0;
      }
    }
    return true;
  }

  if (powerTransitionState == 2) {
    if (millis() - timerMode >= POWER_OFF_FADE_INTERVAL) {
      timerMode = millis();
      if (shutdownFadeBrightness > 0) shutdownFadeBrightness--;
      uint8_t out = crt3_8(shutdownFadeBrightness);
      setLedsBrightness(out, out, out, out);
      if (shutdownFadeBrightness == 0) {
        powerTransitionState = 0;
        setLedsOff();
      }
    }
    return true;
  }

  return false;
}
