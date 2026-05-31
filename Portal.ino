

const char* const* getModeLabels() {
  // Формат GP.SELECT: массив C-строк, конец списка — nullptr
  static const char* const labels[] = {
    getModeLabel(0), getModeLabel(1), getModeLabel(2), getModeLabel(3),
    getModeLabel(4), getModeLabel(5), getModeLabel(6), getModeLabel(7),
    getModeLabel(8), getModeLabel(9), getModeLabel(10), getModeLabel(11),
    getModeLabel(12),
    nullptr
  };
  return labels;
}

const char* getAutoModeLabel(uint8_t i) {
  static const char* labels[] = {"Режим 1", "Режим 2", "Режим 3", "Режим 4", "Режим 5", "Режим 6", "Режим 7", "Режим 8", "Режим 9", "Режим 10", "Режим 11"};
  return (i < 11) ? labels[i] : "Режим";
}

const char* getAutoModeKey(uint8_t i) {
  static const char* keys[] = {"autoM0", "autoM1", "autoM2", "autoM3", "autoM4", "autoM5", "autoM6", "autoM7", "autoM8", "autoM9", "autoM10"};
  return (i < 11) ? keys[i] : "autoM0";
}

void settingsSliderRow(const char* label, const char* name, int value, int minValue, int maxValue) {
  M_BOX(GP_JUSTIFY,
    GP.LABEL(label);
    GP.SEND(F("<span style='display:inline-flex;justify-content:center;align-items:center;width:72%;'>"));
    GP.SLIDER(name, value, minValue, maxValue);
    GP.SEND(F("</span>"));
  );
}
void build() {
  GP.BUILD_BEGIN(GP_DARK);
  GP.PAGE_TITLE("PLAYSTATION LOGO");
  GP.UPDATE("power,brightness,mode,modeBrightnessLimit,segTriangle,segCircle,segX,segSquare");
  GP.ONLINE_CHECK(5000);
  GP.RELOAD_CLICK("useLocAdd,mode");

  if (enableAP) {
    GP.FORM_BEGIN("/connect");
    M_GRID(
      M_BLOCK_TAB(
        "ПОДКЛЮЧЕНИЕ К WIFI",
        GP.TEXT("lg", "Точка доступа", data.ssid, "", 20);
        GP.BREAK();
        GP.PASS_EYE("ps", "Пароль", data.pass, "", 20);
        GP.BREAK();
        GP.HR(GP_GRAY);
        GP.BREAK();
        M_BOX(GP.LABEL("Использовать локальный адрес"); GP.CHECK("useLocAdd", data.useLocalAddress););
        if (useLocalAddress) {
          M_BOX(GP.SPAN("http://", GP_RIGHT); GP.TEXT("la", "Адрес", data.localAddress, "", 20); GP.SPAN(".local", GP_LEFT););
        }
        GP.BREAK();
      );
    );
    GP.SUBMIT("Сохранить");
    GP.FORM_END();
  } else {
    GP.UI_MENU("PlayStation Logo", GP_BLUE);
    GP.UI_LINK("/", "Состояние");
    GP.UI_LINK("/settings", "Настройки");
    GP.UI_LINK("/connect", "Подключение");
    GP.UI_LINK("/ota_update", "Обновление");

    GP.UI_BODY();
    GP.GRID_RESPONSIVE(700);

    if (ui.uri("/")) {
      uint8_t modeIndex = constrain(currentMode, 0, MODE_COUNT - 1);
      M_GRID(
        M_BLOCK_TAB(
          "СОСТОЯНИЕ",
          GP.SEND(F("<div style='display:flex;flex-direction:column;gap:18px;'>"));
          M_BOX(GP_JUSTIFY, GP.LABEL("Питание"); GP.SWITCH("power", powerOn););
          settingsSliderRow("Глобальная яркость", "brightness", globalBrightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
          M_BOX(GP_JUSTIFY,
            GP.LABEL("Режим");
            GP.SEND(F("<span style='display:inline-flex;justify-content:center;align-items:center;width:72%;'>"));
            GP.SELECT("mode", (char**)getModeLabels(), modeIndex, false);
            GP.SEND(F("</span>"));
          );
          GP.SEND(F("<div style='display:flex;justify-content:center;margin-top:6px;'>"));
          GP.BUTTON("btnSaveSettings", "Сохранить");
          GP.SEND(F("</div></div>"));
        );
      );
    } else if (ui.uri("/settings")) {
      uint8_t modeIndex = constrain(currentMode, 0, MODE_COUNT - 1);
      M_GRID(
        M_BLOCK_TAB(
          "НАСТРОЙКИ",
          GP.LABEL("Глобальная яркость");
          GP.BREAK();
          GP.BREAK();
          settingsSliderRow("Глобальная яркость", "brightness", globalBrightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
          GP.BREAK();
          GP.HR(); 
          GP.BREAK();
          GP.LABEL("Яркость сегментов");
          GP.BREAK();
          GP.BREAK();
          settingsSliderRow("Треугольник", "segTriangle", segmentBrightness[0], MIN_BRIGHTNESS, MAX_BRIGHTNESS);
          GP.BREAK();
          settingsSliderRow("Круг", "segCircle", segmentBrightness[1], MIN_BRIGHTNESS, MAX_BRIGHTNESS);
          GP.BREAK();
          settingsSliderRow("Крест", "segX", segmentBrightness[2], MIN_BRIGHTNESS, MAX_BRIGHTNESS);
          GP.BREAK();
          settingsSliderRow("Квадрат", "segSquare", segmentBrightness[3], MIN_BRIGHTNESS, MAX_BRIGHTNESS);
          GP.BREAK();
          GP.HR(); 
          GP.BREAK();
          GP.LABEL("Настройка режимов");
          GP.BREAK();
          GP.BREAK();
          M_BOX(GP.LABEL("Режим"); GP.SELECT("mode", (char**)getModeLabels(), modeIndex, false););
          GP.BREAK();
          if (modeIndex <= 10) {
            settingsSliderRow("Яркость режима", "modeBrightnessLimit", modeBrightnessLimit[modeIndex], MIN_BRIGHTNESS, MAX_BRIGHTNESS);
            GP.BREAK();
          }
          if (modeIndex == 1 || modeIndex == 2 || modeIndex == 3 || modeIndex == 9 || modeIndex == 10) {
            settingsSliderRow("Скорость включения", "modeSpeed", modeSpeed[modeIndex], 1, 5);
            GP.BREAK();
            settingsSliderRow("Скорость выключения", "modeSpeedOff", modeSpeedOff[modeIndex], 1, 5);
            GP.BREAK();
            settingsSliderRow("Пауза после включения", "modePauseOn", modePauseOn[modeIndex], 1, 5);
            GP.BREAK();
            settingsSliderRow("Пауза после выключения", "modePauseOff", modePauseOff[modeIndex], 1, 5);
            GP.BREAK();
          } else if (modeIndex >= 4 && modeIndex <= 8) {
            settingsSliderRow("Скорость эффекта", "modeSpeed", modeSpeed[modeIndex], 1, 5);
            GP.BREAK();
          }
          if (modeIndex == 11 || modeIndex == 12) {
            M_BOX(GP.LABEL("Время смены режима (сек)"); GP.SPINNER("autoPeriodSec", modeSpeed[modeIndex], 1, 60, 1););
            GP.BREAK();
            GP.LABEL("Список режимов для смены");
            for (uint8_t i = 0; i < 11; i++) {
              M_BOX(GP.LABEL(getAutoModeLabel(i)); GP.CHECK(getAutoModeKey(i), modeEnabledInAuto[i]););
            }
            GP.BREAK();
          }
          GP.BUTTON("btnSaveSettings", "Сохранить");
        );
      );
    } else if (ui.uri("/connect")) {
      GP.FORM_BEGIN("/connect");
      M_GRID(
        M_BLOCK_TAB(
          "ПОДКЛЮЧЕНИЕ К WIFI",
          GP.TEXT("lg", "Точка доступа", data.ssid, "", 20);
          GP.BREAK();
          GP.PASS_EYE("ps", "Пароль", data.pass, "", 20);
          GP.BREAK();
          GP.HR(GP_GRAY);
          GP.BREAK();
          M_BOX(GP.LABEL("Использовать локальный адрес"); GP.CHECK("useLocAdd", data.useLocalAddress););
          if (useLocalAddress) {
            M_BOX(GP.SPAN("http://"); GP.TEXT("la", "Адрес", data.localAddress, "", 20); GP.SPAN(".local"););
          }
          GP.BREAK();
        );
      );
      GP.SUBMIT("Сохранить");
      GP.FORM_END();
    } else if (ui.uri("/ota_update")) {
      M_GRID(
        M_BLOCK_TAB(
          "ОБНОВЛЕНИЕ",
          GP.OTA_FIRMWARE("Обновить прошивку");
          GP.BREAK();
          GP.OTA_FILESYSTEM("Обновить файловую систему");
          GP.BREAK();
        );
      );
    }

    GP.UI_END();
  }

  GP.BUILD_END();
}
