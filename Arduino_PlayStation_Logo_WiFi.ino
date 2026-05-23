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
#define MIN_BRIGHTNESS_FADE_MODE 10  //Минимальная яркость в режиме плавного затухания 255-0
#define STEP_AUTO_BRIGHTNESS 5  //Шаг регулировки яркости по удержанию кнопки
#define STEP_MANUAL_BRIGHTNESS 10  //Шаг регулировки яркости по 1 нажатию
#define FADE_TIME_MODE_1 20 // Время увеличения и уменьшения яркости в мс (Default 20 мс)
#define FADE_TIME_MODE_2 5 // Время увеличения и уменьшения яркости в мс (Default 20 мс)
#define PAUSE_TIME_MODE_2 100 // Время паузы между 

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

int ledPin[] = {LED_PIN_TRIANGLE, LED_PIN_CIRCLE, LED_PIN_X, LED_PIN_SQUARE};
int countLedsPin = sizeof(ledPin) / sizeof(int);

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
uint32_t previousMillis, currentMillis, timerMode;

struct Data {
  bool powerOn = false;
  uint8_t globalBrightness = 100;
  uint8_t currentMode = 0;
};

Data data;  // переменная, с которой мы работаем в программе

EEManager memory(data, 10000); // передаём нашу переменную (фактически её адрес)

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
      globalBrightness += STEP_MANUAL_BRIGHTNESS;
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
      globalBrightness += STEP_AUTO_BRIGHTNESS;
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
      globalBrightness -= STEP_MANUAL_BRIGHTNESS;
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
      globalBrightness -= STEP_AUTO_BRIGHTNESS;
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
    if (currentMode > 3) currentMode = 0;
    switch(currentMode){
      case 0:
        setLedsOff();
        Serial.print("Mode = ");
        Serial.println(currentMode);
      break;
      case 1:
        setLedsOff();
        timerMode = millis();
        directionBrightness = true;
        valueBrightness = 0;
        Serial.print("Mode = ");
        Serial.println(currentMode);
      break;
      case 2:
        setLedsOff();
        counterBrightness = 0;
        val = 0;
        flag = true;
        directionBrightness = true;
        timerMode = millis();
        Serial.print("Mode = ");
        Serial.println(currentMode);
      break;
      case 3:
        setLedsOff();
        counterBrightness = 0;
        val = countLedsPin;
        flag = true;
        directionBrightness = true;
        timerMode = millis();
        Serial.print("Mode = ");
        Serial.println(currentMode);
      break;
      case 4:
      case 5:
      case 100:
        setLedsOff();
        timerMode = millis();
        k = 0;
        flag = true;
        Serial.print("Mode = ");
        Serial.println(currentMode);
      default:
        setLedsOff();
        timerMode = millis();
        k = 0;
        flag = true;
        Serial.print("Mode = ");
        Serial.println(currentMode);
      break;
    }
    Serial.println("btnMode - click");
    data.currentMode = currentMode;
    memory.update();
  }
/********************************************Основной цикл работы программы**********************************************/
  if (powerOn){
    switch(currentMode){
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
          timerMode = millis();
          if(directionBrightness){
            counterBrightness++;
            flag = true;
            analogWrite(ledPin[val], crt3_8(counterBrightness));
            if (counterBrightness == 255 && flag) {
              counterBrightness = 0;
              val++;
              flag = false;
            }
            if(val == countLedsPin) {
              directionBrightness = !directionBrightness;
              counterBrightness = 255;
            }
          }else{
            if (val <= 0 ){
              counterBrightness--;
              analogWrite(ledPin[val], crt3_8(counterBrightness));
              if (counterBrightness == 0) { 
                directionBrightness = !directionBrightness;
              }
            }else{
              counterBrightness--;
              flag = true;
              analogWrite(ledPin[val], crt3_8(counterBrightness));
              if (counterBrightness == 0 && flag) { 
                counterBrightness = 255;
                val--;
                flag = false;
              }
            }
          }
        }
          break;
      case 3:
        if (millis() - timerMode >= FADE_TIME_MODE_2){
          timerMode = millis();
          if(directionBrightness){
            if(val <= 0 ){
              counterBrightness++;
              analogWrite(ledPin[val], crt3_8(counterBrightness));
              if(counterBrightness == 255) {
                directionBrightness = !directionBrightness;
              }
            }else{
              counterBrightness++;
              flag = true;
              analogWrite(ledPin[val], crt3_8(counterBrightness));
              if (counterBrightness == 255 && flag) {
                counterBrightness = 0;
                val--;
                flag = false;
              }
            }
          }else{
            if (val >= countLedsPin){
              counterBrightness--;
              analogWrite(ledPin[val], crt3_8(counterBrightness));
              if (counterBrightness == 0) { 
                directionBrightness = !directionBrightness;
              }
            }else{
              counterBrightness--;
              flag = true;
              analogWrite(ledPin[val], crt3_8(counterBrightness));
              if (counterBrightness == 0 && flag) { 
                counterBrightness = 255;
                val++;
                flag = false;
              }
            }
          }
        }
          break;
      case 4:
        currentMode = 0;
          break;
      case 5:
        currentMode = 0;
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
  }else{
    setLedsOff(); //Выключаем все диоды
  }
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

// кубическая гамма для 8 бит
uint8_t crt3_8(uint8_t val) {
  return ((uint32_t)(val + 1) * (val + 1) * val) >> 16;
}