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

    int modeValue = currentMode;
    if (ui.clickInt("mode", modeValue)) {
      currentMode = constrain(modeValue, 0, MODE_COUNT - 1);
      oldMode = currentMode;
      initModeState(currentMode);
      data.selectedMode = currentMode;
      data.currentMode = currentMode;
      DEBUG("Mode: ");
      DEBUGLN(currentMode);
      modeIndex = constrain(currentMode, 0, MODE_COUNT - 1);
    }
    int modeBrightness = modeBrightnessLimit[modeIndex];
    if (ui.clickInt("modeBrightnessLimit", modeBrightness)) {
      modeBrightnessLimit[modeIndex] = constrain(modeBrightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    }

    int modeSpeedVal = modeSpeed[modeIndex];
    if (ui.clickInt("modeSpeed", modeSpeedVal)) {
      modeSpeed[modeIndex] = constrain(modeSpeedVal, 1, 5);
    }
    int modeSpeedOffVal = modeSpeedOff[modeIndex];
    if (ui.clickInt("modeSpeedOff", modeSpeedOffVal)) {
      modeSpeedOff[modeIndex] = constrain(modeSpeedOffVal, 1, 5);
    }
    int modePauseOnVal = modePauseOn[modeIndex];
    if (ui.clickInt("modePauseOn", modePauseOnVal)) {
      modePauseOn[modeIndex] = constrain(modePauseOnVal, 1, 5);
    }
    int modePauseOffVal = modePauseOff[modeIndex];
    if (ui.clickInt("modePauseOff", modePauseOffVal)) {
      modePauseOff[modeIndex] = constrain(modePauseOffVal, 1, 5);
    }
    int autoPeriodMin = modeSpeed[modeIndex];
    if (ui.clickInt("autoPeriodMin", autoPeriodMin) && (modeIndex == 11 || modeIndex == 12)) {
      modeSpeed[modeIndex] = constrain(autoPeriodMin, 1, 60);
    }

    for (uint8_t i = 0; i < 11; i++) {
      char autoModeKey[8];
      snprintf(autoModeKey, sizeof(autoModeKey), "autoM%u", i);
      bool enabled = modeEnabledInAuto[i];
      if (ui.clickBool(autoModeKey, enabled)) modeEnabledInAuto[i] = enabled;
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
      memcpy(data.modeSpeedOff, modeSpeedOff, sizeof(modeSpeedOff));
      memcpy(data.modePauseOn, modePauseOn, sizeof(modePauseOn));
      memcpy(data.modePauseOff, modePauseOff, sizeof(modePauseOff));
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
    ui.updateInt("modeVal", currentMode + 1);
    String modeLabel = getModeLabel(currentMode);
    ui.updateString("modeValText", modeLabel);
    if (ui.uri("/settings") && modeIndex <= 10) {
      ui.updateInt("modeBrightnessLimit", modeBrightnessLimit[modeIndex]);
      ui.updateInt("modeBrightnessLimitVal", modeBrightnessLimit[modeIndex]);
    }
  }
}
