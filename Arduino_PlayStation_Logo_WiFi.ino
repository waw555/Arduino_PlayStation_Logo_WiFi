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

#include <GyverPortal.h>                    //Портал
#include <EEPROM.h>                         //Память для сохранения настроек
#include <LittleFS.h>                       //Файловая система
#include <PubSubClient.h>
#include <EncButton.h>                      //Библиотека для кнопки

//ОТЛАДКА
#define DEBUG_ENABLE //Расскоментируй для включения отладки в порт

#ifdef DEBUG_ENABLE
#define DEBUG(x) Serial.print(x)
#define DEBUGLN(x) Serial.println(x)
#else
#define DEBUG(x)
#endif

#define INIT_ADDR 500  // номер резервной ячейки для инициализации и первой проверки памяти
#define INIT_KEY 53     // При любых изменениях в структурированных данных измените ключ  или стирайте память при прошивке.

//ВЫХОДЫ НА ПОДСВЕТКУ
#define LED_PIN_TRIANGLE 12     //  Пин - Треугольник
#define LED_PIN_CIRCLE 14       //  Пин - Круг
#define LED_PIN_X 15            //  Пин - Икс
#define LED_PIN_SQUARE 4        //  Пин - Квадрат
//КНОПКИ
#define BUTTON_PIN_ON 0         //  Пин - Кнопка Вкл/Выкл
#define BUTTON_PIN_PREV 2       //  Пин - Кнопка Назад
#define BUTTON_PIN_NEXT 13      //  Пин - Кнопка Вперед
#define BUTTON_PIN_MODE 5       //  Пин - Кнопка Режим

//ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ
#define MAX_BRIGHTNESS 255                  //Максимальная яркость 0-255
#define MIN_BRIGHTNESS 5                    //Минимальная яркость 255-0
#define MIN_BRIGHTNESS_FADE_MODE 0          //Минимальная яркость в режиме плавного затухания 255-0
#define STEP_AUTO_BRIGHTNESS 5              //Шаг регулировки яркости по удержанию кнопки
#define STEP_MANUAL_BRIGHTNESS 10           //Шаг регулировки яркости по 1 нажатию
#define FADE_TIME_MODE_1 20                 // Время увеличения и уменьшения яркости в мс (Default 20 мс)
#define FADE_TIME_MODE_2 5                  // Время увеличения и уменьшения яркости в мс (Default 20 мс)
#define HOLD_ON_TIME_MODE_2 5000UL          // Пауза после полного включения фигур (5 секунд)
#define HOLD_OFF_TIME_MODE_2 5000UL         // Пауза после полного выключения фигур (5 секунд)
#define RANDOM_FADE_INTERVAL_MODE_4 8UL     // 
#define RANDOM_DELAY_MIN_MODE_4 150UL       //
#define RANDOM_DELAY_MAX_MODE_4 1200UL      //
#define PULSE_INTERVAL_MODE_5_MIN 5UL       //  
#define PULSE_INTERVAL_MODE_5_MAX 20UL      //
#define LIGHTNING_FADE_INTERVAL 12UL        //
#define LIGHTNING_FADE_STEP_DOWN_MODE_7_8 1 //
#define POWER_OFF_FADE_INTERVAL 10UL        //
#define AUTO_MODE_INTERVAL 5000UL           //Минимальное время изменения режимов в автоматическом режиме (5 секунд)
#define MODE_COUNT 13
#define AUTO_MODES_MAX 11

Button btnOn(BUTTON_PIN_ON, INPUT_PULLUP);      // Вкл/Выкл - btnOn
Button btnPrev(BUTTON_PIN_PREV, INPUT_PULLUP);  // Назад - btnPrev
Button btnNext(BUTTON_PIN_NEXT, INPUT_PULLUP);  // Веперд - btnNext
Button btnMode(BUTTON_PIN_MODE, INPUT_PULLUP);  // Режим btnMode

/**************************ПЕРЕМЕННЫЕ*************************************/
const uint8_t ledPin[] = {LED_PIN_TRIANGLE, LED_PIN_CIRCLE, LED_PIN_X, LED_PIN_SQUARE};
const uint8_t countLedsPin = sizeof(ledPin) / sizeof(ledPin[0]);

bool powerOn = false;
bool dir = true;
bool directionBrightness = true;
bool flag = true;
bool mode8DirectionUp[4] = {true, true, true, true};
bool mode8Active[4] = {false, false, false, false};
bool mode6DirectionUp[4] = {true, true, true, true};
bool pulseDirectionUp[4] = {true, true, true, true};
bool randomLedDirectionUp = true;

uint8_t globalBrightness = 255;
uint8_t valueBrightness = 0;
uint8_t counterBrightness = 0;
uint8_t currentMode = 0;
uint8_t oldMode = 0;
uint8_t val = 0;
uint8_t k = 0;
uint8_t mode2Step = 0;
uint8_t randomLedIndex = 0;
uint8_t randomLedBrightness[4] = {0, 0, 0, 0};
uint8_t pulseBrightness[4] = {0, 0, 0, 0};
uint8_t shuffledLedOrder[4] = {0, 1, 2, 3};
uint8_t shuffledLedOrderIndex = 0;
uint8_t mode6Brightness[4] = {0, 0, 0, 0};
uint8_t mode8Brightness[4] = {0, 0, 0, 0};
uint8_t autoModeCurrent = 0;
uint8_t powerTransitionState = 0; // 0 - нет перехода, 1 - мерцание при включении, 2 - затухание при выключении
uint8_t startupFlickerStep = 0;
uint8_t shutdownFadeBrightness = 0;
uint16_t pulseLedInterval[4] = {0, 0, 0, 0};
uint16_t mode6Interval[4] = {0, 0, 0, 0};
uint32_t mode6Timer[4] = {0, 0, 0, 0};
uint32_t pulseLedTimer[4] = {0, 0, 0, 0};
uint32_t autoModeTimer = 0;
uint32_t randomModeDelayStart = 0;
uint32_t previousMillis, currentMillis, timerMode, randomModeDelay = 0;
uint8_t autoModeOrder[AUTO_MODES_MAX] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
uint8_t autoModeOrderCount = AUTO_MODES_MAX;
uint8_t autoModeOrderIndex = 0;
uint8_t modeBrightnessLimit[MODE_COUNT] = {200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200};
uint8_t segmentBrightness[countLedsPin] = {140, MAX_BRIGHTNESS, 140, 140};
uint8_t modeSpeed[MODE_COUNT] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 10, 10};
uint8_t modeSpeedOff[MODE_COUNT] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
uint8_t modePauseOn[MODE_COUNT] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
uint8_t modePauseOff[MODE_COUNT] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
bool modeEnabledInAuto[MODE_COUNT] = {true, true, true, true, true, true, true, true, true, true, true, false, false};

/**************************СТРУКТУРА НАСТРОЕК В ПАМЯТИ EEPROM*************************************/
struct SettingsData {
  bool powerOn = false;               //Состояние при включении/выключении
  uint8_t globalBrightness = 255;     //Глобальная яркость
  uint8_t currentMode = 0;            //Текущий режим
  char ssid[20] = "SSID";             //SSID
  char pass[20] = "PASSWORD";         //Пароль
  bool useLocalAddress = false;       //Использовать локальный адрес или нет
  char localAddress[20] = "pslogo";     //Локальный адрес http://pslogo.local
  uint8_t selectedMode = 0;
  uint8_t modeBrightnessLimit[MODE_COUNT] = {200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200};
  uint8_t segmentBrightness[4] = {140, MAX_BRIGHTNESS, 140, 140};
  uint8_t modeSpeed[MODE_COUNT] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 10, 10};
  uint8_t modeSpeedOff[MODE_COUNT] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
  uint8_t modePauseOn[MODE_COUNT] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
  uint8_t modePauseOff[MODE_COUNT] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
  bool modeEnabledInAuto[MODE_COUNT] = {true, true, true, true, true, true, true, true, true, true, true, false, false};
  uint8_t autoModeOrder[AUTO_MODES_MAX] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  uint8_t autoModeOrderCount = AUTO_MODES_MAX;
};

SettingsData data;  // переменная, с которой мы работаем в программе

void initModeState(uint8_t mode);
bool handlePowerTransition();
void runCurrentMode(uint8_t modeToRun);
void sanitizeLoadedSettings();
void applyDefaultSettings();
bool saveSettingsToEEPROM();
void clampModeBrightnessLimits();
void clampSegmentBrightness();
void constrainBrightnessSettings();
uint8_t getModeBrightness(uint8_t mode);
uint16_t getModeFadeInterval(uint8_t mode);
uint16_t getModeFadeOffInterval(uint8_t mode);
uint32_t getModePauseOnMs(uint8_t mode);
uint32_t getModePauseOffMs(uint8_t mode);
uint8_t scaleModeBrightness(uint8_t level, uint8_t mode);
uint8_t scaleSegmentBrightness(uint8_t brightness, uint8_t segment);
void writeLedBrightness(uint8_t segment, uint8_t brightness);


const char* getModeLabel(uint8_t modeIndex) {
  static const char* const labels[MODE_COUNT] = {
    "Режим 1", "Режим 2", "Режим 3", "Режим 4", "Режим 5", "Режим 6",
    "Режим 7", "Режим 8", "Режим 9", "Режим 10", "Режим 11", "Случайно", "Автоматически"
  };
  return labels[constrain(modeIndex, 0, MODE_COUNT - 1)];
}

uint32_t time_connect = 0; // временная переменная для расчета времени до запуска точки доступа
uint32_t time_manual_mode = 0; //Переменная времени для ручного режима запуска
bool enableAP = false; // проверка запущена ли точка доступа
bool useLocalAddress = false;

void changePowerState(bool on);
void changeBrightness(int delta);

GyverPortal ui(&LittleFS);

void setup() {
  #ifdef DEBUG_ENABLE
    Serial.begin(9600);
  #endif
  
  DEBUGLN("Start System");
  for(int i = 0; i < countLedsPin; i++){
    pinMode(ledPin[i], OUTPUT);
  }

  // Инициализация памяти
  EEPROM.begin(512);
  if (EEPROM.read(INIT_ADDR) != INIT_KEY) { // Проверяем на первый запуск и отсутстувие по адресу INIT_ADDR ключа INIT_KEY
    EEPROM.write(INIT_ADDR, INIT_KEY);    // Записываем ключ INIT_KEY по адресу INIT_ADDR
    saveSettingsToEEPROM();  //Записываем данные по умолчанию в память
    DEBUGLN("Set Default Data In Memory");
  }
  delay(50);
  EEPROM.get(0, data); // Читаем данные из памяти
  sanitizeLoadedSettings();
  delay(100);
  powerOn = data.powerOn; 
  globalBrightness = data.globalBrightness; 
  currentMode = data.currentMode;
  useLocalAddress = data.useLocalAddress;

  oldMode = currentMode;
  memcpy(modeBrightnessLimit, data.modeBrightnessLimit, sizeof(modeBrightnessLimit));
  memcpy(segmentBrightness, data.segmentBrightness, sizeof(segmentBrightness));
  constrainBrightnessSettings();
  memcpy(modeSpeed, data.modeSpeed, sizeof(modeSpeed));
  memcpy(modeSpeedOff, data.modeSpeedOff, sizeof(modeSpeedOff));
  memcpy(modePauseOn, data.modePauseOn, sizeof(modePauseOn));
  memcpy(modePauseOff, data.modePauseOff, sizeof(modePauseOff));
  memcpy(modeEnabledInAuto, data.modeEnabledInAuto, sizeof(modeEnabledInAuto));
  memcpy(autoModeOrder, data.autoModeOrder, sizeof(autoModeOrder));
  autoModeOrderCount = data.autoModeOrderCount;
  randomSeed(micros());
  initModeState(currentMode);

 // пытаемся подключиться
  DEBUG("Connect to: ");
  DEBUGLN(data.ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(data.ssid, data.pass);
  time_connect = millis();  //записываем текущее время
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DEBUG(".");
    
    if(millis() - time_connect >= 10000 && !enableAP){ // Если время вышло и точка доступа не запущена, то запускаем точку доступа с формой ввода SSID и пароля 
      enableAP = true;
      startAPAndFormForConnectToWIFI();
    }
  }
  DEBUGLN();
  DEBUG("Connected! Local IP: ");

  DEBUGLN(WiFi.localIP());

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  if (!LittleFS.begin()) {
    DEBUGLN("FS Error");
  }

  //Если подключились к точке доступа, то запускаем портал.
  ui.attachBuild(build);
  ui.attach(action);
  if(useLocalAddress){
    ui.start(data.localAddress);
    DEBUGLN();
    DEBUG("Portal Start: ");
    DEBUG(data.localAddress);
    DEBUGLN();    
  }else{
    ui.start();
    DEBUGLN();
    DEBUG("Portal Start");
    DEBUGLN(); 
  } 
  ui.enableOTA();   // OTA обновление прошивки без пароля
  //ui.enableOTA("admin", "pass");  // с паролем
  ui.downloadAuto(true);
}

bool saveSettingsToEEPROM() {
  EEPROM.put(0, data);
  if (EEPROM.commit()) {
    DEBUGLN("EEPROM successfully committed");
    return true;
  }

  DEBUGLN("ERROR! EEPROM commit failed");
  return false;
}

void applyDefaultSettings() {
  globalBrightness = MAX_BRIGHTNESS;
  segmentBrightness[0] = 140;
  segmentBrightness[1] = MAX_BRIGHTNESS;
  segmentBrightness[2] = 140;
  segmentBrightness[3] = 140;

  for (uint8_t i = 0; i < MODE_COUNT; i++) {
    modeBrightnessLimit[i] = 200;
    modeSpeed[i] = (i >= 11) ? 10 : 3;
    modeSpeedOff[i] = 3;
    modePauseOn[i] = 3;
    modePauseOff[i] = 3;
    modeEnabledInAuto[i] = i < AUTO_MODES_MAX;
  }

  autoModeOrderCount = AUTO_MODES_MAX;
  for (uint8_t i = 0; i < AUTO_MODES_MAX; i++) {
    autoModeOrder[i] = i;
  }

  data.powerOn = powerOn;
  data.globalBrightness = globalBrightness;
  data.currentMode = currentMode;
  data.useLocalAddress = useLocalAddress;
  data.selectedMode = currentMode;
  memcpy(data.modeBrightnessLimit, modeBrightnessLimit, sizeof(modeBrightnessLimit));
  memcpy(data.segmentBrightness, segmentBrightness, sizeof(segmentBrightness));
  memcpy(data.modeSpeed, modeSpeed, sizeof(modeSpeed));
  memcpy(data.modeSpeedOff, modeSpeedOff, sizeof(modeSpeedOff));
  memcpy(data.modePauseOn, modePauseOn, sizeof(modePauseOn));
  memcpy(data.modePauseOff, modePauseOff, sizeof(modePauseOff));
  memcpy(data.modeEnabledInAuto, modeEnabledInAuto, sizeof(modeEnabledInAuto));
  memcpy(data.autoModeOrder, autoModeOrder, sizeof(autoModeOrder));
  data.autoModeOrderCount = autoModeOrderCount;
}

void sanitizeLoadedSettings() {
  data.currentMode = constrain(data.currentMode, 0, MODE_COUNT - 1);
  data.selectedMode = constrain(data.selectedMode, 0, MODE_COUNT - 1);
  data.globalBrightness = constrain(data.globalBrightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS);

  for (uint8_t i = 0; i < MODE_COUNT; i++) {
    data.modeBrightnessLimit[i] = constrain(data.modeBrightnessLimit[i], MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    data.modeSpeed[i] = (i >= 11) ? constrain(data.modeSpeed[i], 1, 60) : constrain(data.modeSpeed[i], 1, 5);
    data.modeSpeedOff[i] = constrain(data.modeSpeedOff[i], 1, 5);
    data.modePauseOn[i] = constrain(data.modePauseOn[i], 1, 5);
    data.modePauseOff[i] = constrain(data.modePauseOff[i], 1, 5);
  }

  for (uint8_t i = 0; i < 4; i++) {
    data.segmentBrightness[i] = constrain(data.segmentBrightness[i], MIN_BRIGHTNESS, MAX_BRIGHTNESS);
  }

  data.autoModeOrderCount = constrain(data.autoModeOrderCount, 0, AUTO_MODES_MAX);
  for (uint8_t i = 0; i < AUTO_MODES_MAX; i++) {
    data.autoModeOrder[i] = constrain(data.autoModeOrder[i], 0, MODE_COUNT - 2);
  }
}

void clampModeBrightnessLimits() {
  for (uint8_t i = 0; i < MODE_COUNT; i++) {
    modeBrightnessLimit[i] = constrain(modeBrightnessLimit[i], MIN_BRIGHTNESS, MAX_BRIGHTNESS);
  }
}

void clampSegmentBrightness() {
  for (uint8_t i = 0; i < countLedsPin; i++) {
    segmentBrightness[i] = constrain(segmentBrightness[i], MIN_BRIGHTNESS, MAX_BRIGHTNESS);
  }
}

void constrainBrightnessSettings() {
  globalBrightness = constrain(globalBrightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
  clampModeBrightnessLimits();
  clampSegmentBrightness();
}

uint8_t getModeBrightness(uint8_t mode) {
  uint8_t modeIndex = constrain(mode, 0, MODE_COUNT - 1);
  return min<uint8_t>(modeBrightnessLimit[modeIndex], globalBrightness);
}

uint16_t getModeFadeInterval(uint8_t mode) {
  uint8_t seconds = constrain(modeSpeed[constrain(mode, 0, MODE_COUNT - 1)], 1, 5);
  return max<uint16_t>(1, ((uint16_t)seconds * 1000UL) / 255UL);
}

uint16_t getModeFadeOffInterval(uint8_t mode) {
  uint8_t seconds = constrain(modeSpeedOff[constrain(mode, 0, MODE_COUNT - 1)], 1, 5);
  return max<uint16_t>(1, ((uint16_t)seconds * 1000UL) / 255UL);
}

uint32_t getModePauseOnMs(uint8_t mode) {
  return (uint32_t)constrain(modePauseOn[constrain(mode, 0, MODE_COUNT - 1)], 1, 5) * 1000UL;
}

uint32_t getModePauseOffMs(uint8_t mode) {
  return (uint32_t)constrain(modePauseOff[constrain(mode, 0, MODE_COUNT - 1)], 1, 5) * 1000UL;
}

uint8_t scaleModeBrightness(uint8_t level, uint8_t mode) {
  return ((uint16_t)level * getModeBrightness(mode)) / MAX_BRIGHTNESS;
}

uint8_t scaleSegmentBrightness(uint8_t brightness, uint8_t segment) {
  uint8_t segmentIndex = constrain(segment, 0, countLedsPin - 1);
  return ((uint16_t)brightness * segmentBrightness[segmentIndex]) / MAX_BRIGHTNESS;
}

void writeLedBrightness(uint8_t segment, uint8_t brightness) {
  uint8_t segmentIndex = constrain(segment, 0, countLedsPin - 1);
  analogWrite(ledPin[segmentIndex], scaleSegmentBrightness(brightness, segmentIndex));
}
/*******************Запускаем портал с формой для подключения к WiFi************************************/
void startAPAndFormForConnectToWIFI() {
  DEBUGLN("Start AP and Portal");

  // запускаем точку доступа
  WiFi.mode(WIFI_AP);
  WiFi.softAP("PS_LOGO_AP");

  // запускаем портал с формой ввода
  ui.attachBuild(build);
  ui.attach(action);
  if(useLocalAddress){
    ui.start(data.localAddress);
    DEBUGLN();
    DEBUG("Portal Start: ");
    DEBUG(data.localAddress);
    DEBUGLN();    
  }else{
    ui.start();
    DEBUGLN();
    DEBUG("Portal Start");
    DEBUGLN(); 
  } 

  // работа портала
  while (ui.tick());
}

void loop() {
  ui.tick(); //Вызываем портал
  btnOn.tick();
  btnNext.tick();
  btnPrev.tick();
  btnMode.tick();

  // запоминаем время
  currentMillis = millis();   // текущее время в миллисекундах

  //Если нажали кнопку включения, то меняем значение powerOn на противоположное
  if (btnOn.click()) {
    changePowerState(!powerOn);
    Serial.println("btnOn - click");
  }

  //Если нажали кнопку увеличения яркости, то прибавляем яркость на количество STEP_BRIGHTNESS
  if (btnNext.click()) {
    changeBrightness(STEP_MANUAL_BRIGHTNESS);
    Serial.println("btnNext - click");
  }

  //Если удерживаем нажатой кнопку увеличения яркости
  if (btnNext.step()) {
    changeBrightness(STEP_AUTO_BRIGHTNESS);
    Serial.println("btnNext - step");
  }

  //Если нажали кнопку уменьшения яркости, то убавляем яркость на количество STEP_BRIGHTNESS
  if (btnPrev.click()) {
    changeBrightness(-STEP_MANUAL_BRIGHTNESS);
    Serial.println("btnPrev - click");
  }

  //Если удерживаем нажатой кнопку уменьшения яркости
  if (btnPrev.step()) {
    changeBrightness(-STEP_AUTO_BRIGHTNESS);
    Serial.println("btnPrev - step");
  }

/*******************************************Кнопка переключения режимов****************************************************/
  if (btnMode.click()) 
  {
    //oldMode = currentMode;
    currentMode++;
    if (currentMode >= MODE_COUNT) currentMode = 0;
    initModeState(currentMode);
    Serial.println("btnMode - click");
    data.currentMode = currentMode;
    DEBUGLN("Save Settings");
    saveSettingsToEEPROM();
  }
/********************************************Основной цикл работы программы**********************************************/
  if (powerTransitionState != 0 && handlePowerTransition()) return;
  if (powerOn){
    if ((currentMode == 11 || currentMode == 12) && millis() - autoModeTimer >= (uint32_t)modeSpeed[currentMode] * 1000UL) {
      if (currentMode == 11 || autoModeOrderCount == 0) {
        autoModeCurrent = random(0, 11);
      } else {
        autoModeCurrent = autoModeOrder[autoModeOrderIndex % autoModeOrderCount];
        autoModeOrderIndex = (autoModeOrderIndex + 1) % autoModeOrderCount;
      }
      initModeState(autoModeCurrent);
      autoModeTimer = millis();
    }
    uint8_t modeToRun = (currentMode == 11 || currentMode == 12) ? autoModeCurrent : currentMode;
    runCurrentMode(modeToRun);
  }else{
    setLedsOff(); //Выключаем все диоды
  }
}

void runCurrentMode(uint8_t modeToRun) {
  switch(modeToRun){
      case 0: {
        uint8_t brightness = getModeBrightness(modeToRun);
        setLedsBrightness(brightness, brightness, brightness, brightness);
          break;
      }
      case 1: {
        if (mode2Step == 0 && millis() - timerMode >= getModeFadeInterval(modeToRun)) {
          timerMode = millis();
          if (valueBrightness < MAX_BRIGHTNESS) valueBrightness++;
          if (valueBrightness >= MAX_BRIGHTNESS) {
            valueBrightness = MAX_BRIGHTNESS;
            mode2Step = 1;
          }
        } else if (mode2Step == 1 && millis() - timerMode >= getModePauseOnMs(modeToRun)) {
          mode2Step = 2;
          timerMode = millis();
        } else if (mode2Step == 2 && millis() - timerMode >= getModeFadeOffInterval(modeToRun)) {
          timerMode = millis();
          if (valueBrightness > MIN_BRIGHTNESS_FADE_MODE) valueBrightness--;
          if (valueBrightness <= MIN_BRIGHTNESS_FADE_MODE) {
            valueBrightness = MIN_BRIGHTNESS_FADE_MODE;
            mode2Step = 3;
          }
        } else if (mode2Step == 3 && millis() - timerMode >= getModePauseOffMs(modeToRun)) {
          mode2Step = 0;
          timerMode = millis();
        }
        uint8_t out = scaleModeBrightness(valueBrightness, modeToRun);
        setLedsBrightness(out, out, out, out);
          break;
      }
      case 2:
        if (millis() - timerMode >= ((mode2Step == 2) ? getModeFadeOffInterval(modeToRun) : getModeFadeInterval(modeToRun))){
          if (mode2Step == 0) {
            timerMode = millis();  // Плавно включаем фигуры: Треугольник -> Круг -> Крест -> Квадрат
            counterBrightness++;
            writeLedBrightness(val, crt3_8(scaleModeBrightness(counterBrightness, modeToRun)));
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
              writeLedBrightness(val, crt3_8(scaleModeBrightness(255 - counterBrightness, modeToRun)));
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
        if (mode2Step == 1 && millis() - timerMode >= getModePauseOnMs(modeToRun)) {  // Пауза во включенном состоянии
          mode2Step = 2;
          val = countLedsPin - 1;
          counterBrightness = 0;
        } else if (mode2Step == 3 && millis() - timerMode >= getModePauseOffMs(modeToRun)) {  // Пауза в выключенном состоянии
          mode2Step = 0;
          val = 0;
          counterBrightness = 0;
        }
          break;
      case 3:
        if (millis() - timerMode >= ((mode2Step == 2) ? getModeFadeOffInterval(modeToRun) : getModeFadeInterval(modeToRun))){
          if (mode2Step == 0) {
            timerMode = millis();  // Плавно включаем фигуры: Квадрат -> Крест -> Круг -> Треугольник
            counterBrightness++;
            writeLedBrightness(val, crt3_8(scaleModeBrightness(counterBrightness, modeToRun)));
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
              writeLedBrightness(val, crt3_8(scaleModeBrightness(255 - counterBrightness, modeToRun)));
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
        if (mode2Step == 1 && millis() - timerMode >= getModePauseOnMs(modeToRun)) {  // Пауза во включенном состоянии
          mode2Step = 2;
          val = 0;
          counterBrightness = 0;
        } else if (mode2Step == 3 && millis() - timerMode >= getModePauseOffMs(modeToRun)) {  // Пауза в выключенном состоянии
          mode2Step = 0;
          val = countLedsPin - 1;
          counterBrightness = 0;
        }
          break;
      case 4:
        if (millis() - timerMode >= getModeFadeInterval(modeToRun)) {
          timerMode = millis();

          if (randomLedDirectionUp) {
            if (randomLedBrightness[randomLedIndex] < getModeBrightness(modeToRun)) {
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
          writeLedBrightness(i, crt3_8(randomLedBrightness[i]));
        }
          break;
      case 5:
        for (uint8_t i = 0; i < countLedsPin; i++) {
          if (millis() - pulseLedTimer[i] >= pulseLedInterval[i]) {
            pulseLedTimer[i] = millis();
            if (pulseDirectionUp[i]) {
              if (pulseBrightness[i] < getModeBrightness(modeToRun)) pulseBrightness[i]++;
              else pulseDirectionUp[i] = false;
            } else {
              if (pulseBrightness[i] > 0) pulseBrightness[i]--;
              else pulseDirectionUp[i] = true;
            }

            if (pulseBrightness[i] == 0 || pulseBrightness[i] == getModeBrightness(modeToRun)) {
              pulseLedInterval[i] = random(getModeFadeInterval(modeToRun), getModeFadeInterval(modeToRun) + 16);
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
              if (mode6Brightness[i] < getModeBrightness(modeToRun)) mode6Brightness[i]++;
              else mode6DirectionUp[i] = false;
            } else {
              if (mode6Brightness[i] > 0) mode6Brightness[i]--;
              else mode6DirectionUp[i] = true;
            }
            if (mode6Brightness[i] == 0 || mode6Brightness[i] == getModeBrightness(modeToRun)) {
              mode6Interval[i] = random(getModeFadeInterval(modeToRun), getModeFadeInterval(modeToRun) + 16);
            }
          }
          writeLedBrightness(i, crt3_8(mode6Brightness[i]));
        }
          break;
      case 7:
        if (millis() - timerMode >= getModeFadeInterval(modeToRun)) {
          timerMode = millis();
          if (randomLedBrightness[randomLedIndex] > LIGHTNING_FADE_STEP_DOWN_MODE_7_8) {
            randomLedBrightness[randomLedIndex] -= LIGHTNING_FADE_STEP_DOWN_MODE_7_8;
          } else {
            randomLedBrightness[randomLedIndex] = 0;
            randomLedIndex = random(countLedsPin);
            randomLedBrightness[randomLedIndex] = getModeBrightness(modeToRun);
          }
        }
        for (uint8_t i = 0; i < countLedsPin; i++) writeLedBrightness(i, crt3_8(randomLedBrightness[i]));
          break;
      case 8:
        if (millis() - timerMode >= getModeFadeInterval(modeToRun)) {
          timerMode = millis();

          uint8_t activeCount = 0;
          for (uint8_t i = 0; i < countLedsPin; i++) if (mode8Active[i]) activeCount++;

          if (activeCount == 0) {
            uint8_t targetCount = random(1, countLedsPin + 1);
            while (targetCount > 0) {
              uint8_t idx = random(countLedsPin);
              if (!mode8Active[idx]) {
                mode8Active[idx] = true;
                mode8Brightness[idx] = getModeBrightness(modeToRun);
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
            writeLedBrightness(i, crt3_8(mode8Brightness[i]));
          }
        }
          break;
      case 9:
        if (millis() - timerMode >= ((mode2Step == 2) ? getModeFadeOffInterval(modeToRun) : getModeFadeInterval(modeToRun))){
          if (mode2Step == 0) {
            timerMode = millis();
            counterBrightness++;
            writeLedBrightness(val, crt3_8(scaleModeBrightness(counterBrightness, modeToRun)));
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
              writeLedBrightness(val, crt3_8(scaleModeBrightness(255 - counterBrightness, modeToRun)));
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
        if (mode2Step == 1 && millis() - timerMode >= getModePauseOnMs(modeToRun)) {
          mode2Step = 2;
          val = 0;
          counterBrightness = 0;
        } else if (mode2Step == 3 && millis() - timerMode >= getModePauseOffMs(modeToRun)) {
          mode2Step = 0;
          val = 0;
          counterBrightness = 0;
        }
          break;
      case 10:
        if (millis() - timerMode >= ((mode2Step == 2) ? getModeFadeOffInterval(modeToRun) : getModeFadeInterval(modeToRun))){
          if (mode2Step == 0) {
            timerMode = millis();
            counterBrightness++;
            writeLedBrightness(val, crt3_8(scaleModeBrightness(counterBrightness, modeToRun)));
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
              writeLedBrightness(val, crt3_8(scaleModeBrightness(255 - counterBrightness, modeToRun)));
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
        if (mode2Step == 1 && millis() - timerMode >= getModePauseOnMs(modeToRun)) {
          mode2Step = 2;
          val = countLedsPin - 1;
          counterBrightness = 0;
        } else if (mode2Step == 3 && millis() - timerMode >= getModePauseOffMs(modeToRun)) {
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
      mode2Step = 0;
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
        pulseBrightness[i] = random(getModeBrightness(mode) + 1);
        pulseDirectionUp[i] = random(2);
        pulseLedTimer[i] = millis();
        pulseLedInterval[i] = random(getModeFadeInterval(mode), getModeFadeInterval(mode) + 16);
      }
      break;
    case 6:
      for (uint8_t i = 0; i < countLedsPin; i++) {
        mode6Brightness[i] = random(getModeBrightness(mode) + 1);
        mode6DirectionUp[i] = random(2);
        mode6Timer[i] = millis();
        mode6Interval[i] = random(getModeFadeInterval(mode), getModeFadeInterval(mode) + 16);
      }
      break;
    case 7:
      for (uint8_t i = 0; i < countLedsPin; i++) randomLedBrightness[i] = 0;
      randomLedIndex = random(countLedsPin);
      randomLedBrightness[randomLedIndex] = getModeBrightness(mode);
      randomLedDirectionUp = false;
      break;
    case 8:
      for (uint8_t i = 0; i < countLedsPin; i++) {
        mode8Brightness[i] = 0;
        mode8DirectionUp[i] = false;
        mode8Active[i] = false;
      }
      break;
    case 11:
    case 12:
      autoModeCurrent = random(0, 11);
      autoModeTimer = millis();
      initModeState(autoModeCurrent);
      break;
    case 100:
      k = 0;
      flag = true;
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
  writeLedBrightness(0, ledBrightnessTriangle);
  writeLedBrightness(1, ledBrightnessCircle);
  writeLedBrightness(2, ledBrightnessX);
  writeLedBrightness(3, ledBrightnessSquare);
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

void changePowerState(bool on) {
  if (powerOn == on) return;

  if (on) {
    startupFlickerStep = 0;
    powerTransitionState = 1;
    timerMode = millis();
  } else {
    shutdownFadeBrightness = globalBrightness;
    powerTransitionState = 2;
    timerMode = millis();
  }

  powerOn = on;
  data.powerOn = powerOn;
  DEBUGLN("Save Settings");
  saveSettingsToEEPROM();
}

void changeBrightness(int delta) {
  int target = (int)globalBrightness + delta;
  if (target > MAX_BRIGHTNESS) {
    globalBrightness = MAX_BRIGHTNESS;
    showMinMaxBrightness();
  } else if (target < MIN_BRIGHTNESS) {
    globalBrightness = MIN_BRIGHTNESS;
    showMinMaxBrightness();
  } else {
    globalBrightness = target;
  }

  constrainBrightnessSettings();

  Serial.print("Brightness = ");
  Serial.println(globalBrightness);
  data.globalBrightness = globalBrightness;
  memcpy(data.segmentBrightness, segmentBrightness, sizeof(segmentBrightness));
  DEBUGLN("Save Settings");
  saveSettingsToEEPROM();
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
