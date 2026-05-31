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

    saveSettingsToEEPROM();
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

    int brightness = globalBrightness;
    if (ui.clickInt("brightness", brightness)) {
      globalBrightness = constrain(brightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
      constrainBrightnessSettings();
      data.globalBrightness = globalBrightness;
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

    int segmentValue = segmentBrightness[0];
    if (ui.clickInt("segTriangle", segmentValue)) {
      segmentBrightness[0] = constrain(segmentValue, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    }
    segmentValue = segmentBrightness[1];
    if (ui.clickInt("segCircle", segmentValue)) {
      segmentBrightness[1] = constrain(segmentValue, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    }
    segmentValue = segmentBrightness[2];
    if (ui.clickInt("segX", segmentValue)) {
      segmentBrightness[2] = constrain(segmentValue, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    }
    segmentValue = segmentBrightness[3];
    if (ui.clickInt("segSquare", segmentValue)) {
      segmentBrightness[3] = constrain(segmentValue, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
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
    int autoPeriodSec = modeSpeed[modeIndex];
    if (ui.clickInt("autoPeriodSec", autoPeriodSec) && (modeIndex == 11 || modeIndex == 12)) {
      modeSpeed[modeIndex] = constrain(autoPeriodSec, 1, 60);
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
      memcpy(data.segmentBrightness, segmentBrightness, sizeof(segmentBrightness));
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
      saveSettingsToEEPROM();
    }

    if (ui.click("btnResetSettings")) {
      applyDefaultSettings();
      constrainBrightnessSettings();
      initModeState(currentMode);

      DEBUGLN("Reset Settings");
      saveSettingsToEEPROM();
    }
  }

  if (ui.update()) {
    uint8_t modeIndex = constrain(currentMode, 0, MODE_COUNT - 1);
    ui.updateBool("power", powerOn);
    ui.updateInt("brightness", globalBrightness);
    ui.updateInt("mode", currentMode);
    if (ui.uri("/settings")) {
      ui.updateInt("segTriangle", segmentBrightness[0]);
      ui.updateInt("segCircle", segmentBrightness[1]);
      ui.updateInt("segX", segmentBrightness[2]);
      ui.updateInt("segSquare", segmentBrightness[3]);
    }
    if (ui.uri("/settings") && modeIndex <= 10) {
      ui.updateInt("modeBrightnessLimit", modeBrightnessLimit[modeIndex]);
    }
  }
}
