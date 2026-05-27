void action(GyverPortal& p) {
  if (p.form("/connect")) {
    p.copyStr("lg", data.ssid);
    p.copyStr("ps", data.pass);
    if (useLocalAddress) {
      p.copyStr("la", data.localAddress);
    }
    data.useLocalAddress = useLocalAddress;
    data.powerOn = powerOn;
    data.globalBrightness = globalBrightness;
    data.currentMode = currentMode;

    EEPROM.put(0, data);
    if (EEPROM.commit()) {
      DEBUGLN("EEPROM successfully committed");
    } else {
      DEBUGLN("ERROR! EEPROM commit failed");
    }
    delay(100);
    DEBUGLN("SYSTEM RESTART");
    ESP.restart();
  }

  if (ui.click()) {
    uint8_t modeIndex = constrain(currentMode, 0, MODE_COUNT - 1);
    if (ui.clickBool("power", powerOn)) {
      changePowerState(powerOn);
      DEBUG("Power: ");
      DEBUGLN(powerOn);
    }

    int brightnessValue = globalBrightness;
    if (ui.clickInt("brightness", brightnessValue)) {
      globalBrightness = constrain(brightnessValue, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
      data.globalBrightness = globalBrightness;
      DEBUG("Brightness: ");
      DEBUGLN(globalBrightness);
    }

    int modeValue = currentMode;
    if (ui.clickInt("mode", modeValue)) {
      currentMode = constrain(modeValue, 0, 11);
      oldMode = currentMode;
      initModeState(currentMode);
      data.selectedMode = currentMode;
      data.currentMode = currentMode;
      DEBUG("Mode: ");
      DEBUGLN(currentMode);
    }
    int modeBrightness = modeBrightnessLimit[modeIndex];
    if (ui.clickInt("modeBrightnessLimit", modeBrightness)) {
      modeBrightnessLimit[modeIndex] = constrain(modeBrightness, MIN_BRIGHTNESS, globalBrightness);
    }

    int modeSpeedVal = modeSpeed[modeIndex];
    if (ui.clickInt("modeSpeed", modeSpeedVal)) {
      modeSpeed[modeIndex] = constrain(modeSpeedVal, 1, 10);
    }

    int offTimerMinutes = data.offTimerMinutes;
    if (ui.clickInt("offTimerMinutes", offTimerMinutes)) {
      data.offTimerMinutes = constrain(offTimerMinutes, 0, 1440);
      autoOffDurationMs = (uint32_t)data.offTimerMinutes * 60000UL;
      manualOffStart = millis();
    }

    int offAtMinutes = 0;
    if (ui.clickInt("offAtMinutes", offAtMinutes)) {
      offAtMinutes = constrain(offAtMinutes, 0, 1440);
      autoOffAtMillis = (offAtMinutes == 0) ? 0 : millis() + (uint32_t)offAtMinutes * 60000UL;
    }

    for (uint8_t i = 0; i < 11; i++) {
      String key = String("autoM") + String(i);
      bool enabled = modeEnabledInAuto[i];
      if (ui.clickBool(key, enabled)) modeEnabledInAuto[i] = enabled;
    }

    if (ui.clickBool("useLocAdd", data.useLocalAddress)) {
      useLocalAddress = data.useLocalAddress;
      DEBUG("useLocalAddress: ");
      DEBUGLN(useLocalAddress);
    }

    if (ui.click("btnSaveSettings")) {
      data.powerOn = powerOn;
      data.globalBrightness = globalBrightness;
      data.currentMode = currentMode;
      data.useLocalAddress = useLocalAddress;
      memcpy(data.modeBrightnessLimit, modeBrightnessLimit, sizeof(modeBrightnessLimit));
      memcpy(data.modeSpeed, modeSpeed, sizeof(modeSpeed));
      memcpy(data.modeEnabledInAuto, modeEnabledInAuto, sizeof(modeEnabledInAuto));
      data.autoModeOrderCount = 0;
      for (uint8_t i = 0; i < 11 && data.autoModeOrderCount < AUTO_MODES_MAX; i++) {
        if (modeEnabledInAuto[i]) {
          data.autoModeOrder[data.autoModeOrderCount++] = i;
        }
      }
      memcpy(autoModeOrder, data.autoModeOrder, sizeof(autoModeOrder));
      autoModeOrderCount = data.autoModeOrderCount;

      DEBUGLN("Save Settings");
      EEPROM.put(0, data);
      if (EEPROM.commit()) {
        DEBUGLN("EEPROM successfully committed");
      } else {
        DEBUGLN("ERROR! EEPROM commit failed");
      }
    }
  }

  if (ui.update()) {
    uint8_t modeIndex = constrain(currentMode, 0, MODE_COUNT - 1);
    ui.updateBool("power", powerOn);
    ui.updateInt("brightnessVal", globalBrightness);
    ui.updateInt("modeVal", currentMode);
    ui.updateInt("modeBrightnessLimitVal", modeBrightnessLimit[modeIndex]);
  }
}
