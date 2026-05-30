

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
void build() {
  GP.BUILD_BEGIN(GP_DARK);
  GP.PAGE_TITLE("PLAYSTATION LOGO");
  GP.UPDATE("power,brightnessVal,modeVal,modeValText");
  GP.ONLINE_CHECK(5000);
  GP.RELOAD_CLICK("useLocAdd");

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
      M_GRID(
        M_BLOCK_TAB(
          "СОСТОЯНИЕ",
          M_BOX(GP.LABEL("Питание"); GP.SWITCH("power", powerOn););
          GP.BREAK();
          M_BOX(GP.LABEL("Яркость"); GP.LABEL_BLOCK(String(globalBrightness), "brightnessVal"););
          GP.BREAK();
          M_BOX(GP.LABEL("Режим"); GP.LABEL_BLOCK(String(currentMode + 1), "modeVal"); GP.LABEL_BLOCK(getModeLabel(currentMode), "modeValText"););
        );
      );
    } else if (ui.uri("/settings")) {
      uint8_t modeIndex = constrain(currentMode, 0, MODE_COUNT - 1);
      M_GRID(
        M_BLOCK_TAB(
          "НАСТРОЙКИ",
          M_BOX(GP.LABEL("Максимальная яркость"); GP.SLIDER("brightness", globalBrightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS););
          GP.BREAK();
          M_BOX(GP.LABEL("Режим"); GP.SELECT("mode", (char**)getModeLabels(), modeIndex, false););
          GP.BREAK();
          M_BOX(GP.LABEL("Яркость режима"); GP.SLIDER("modeBrightnessLimit", modeBrightnessLimit[modeIndex], MIN_BRIGHTNESS, globalBrightness); GP.LABEL_BLOCK(String(modeBrightnessLimit[modeIndex]), "modeBrightnessLimitVal"););
          GP.BREAK();
          if (modeIndex == 1) {
            M_BOX(GP.LABEL("Скорость включения"); GP.SLIDER("modeSpeed", modeSpeed[modeIndex], 1, 10););
            GP.BREAK();
            M_BOX(GP.LABEL("Скорость выключения"); GP.SLIDER("modeSpeedOff", modeSpeed[modeIndex], 1, 10););
            GP.BREAK();
            M_BOX(GP.LABEL("Пауза после включения"); GP.SLIDER("modePauseOn", modeSpeed[modeIndex], 1, 10););
            GP.BREAK();
            M_BOX(GP.LABEL("Пауза после выключения"); GP.SLIDER("modePauseOff", modeSpeed[modeIndex], 1, 10););
            GP.BREAK();
          } else if (modeIndex >= 2 && modeIndex <= 11) {
            M_BOX(GP.LABEL("Скорость эффекта"); GP.SLIDER("modeSpeed", modeSpeed[modeIndex], 1, 10););
            GP.BREAK();
          }
          if (modeIndex == 11 || modeIndex == 12) {
            M_BOX(GP.LABEL("Время смены режима (мин)"); GP.SPINNER("autoPeriodMin", modeSpeed[modeIndex], 1, 60, 1););
            GP.BREAK();
            GP.LABEL("Список режимов для смены");
            for (uint8_t i = 0; i < 11; i++) {
              M_BOX(GP.LABEL(getAutoModeLabel(i)); GP.CHECK(getAutoModeKey(i), modeEnabledInAuto[i]););
            }
            GP.BREAK();
          }
          M_BOX(GP.LABEL("Таймер отключения (мин)"); GP.SPINNER("offTimerMinutes", data.offTimerMinutes, 0, 1440, 5););
          GP.BREAK();
          M_BOX(GP.LABEL("Отключить в (мин от старта)"); GP.SPINNER("offAtMinutes", 0, 0, 1440, 5););
          GP.BREAK();
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
