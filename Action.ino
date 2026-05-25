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
      currentMode = constrain(modeValue, 0, 8);
      oldMode = currentMode;
      initModeState(currentMode);
      data.currentMode = currentMode;
      DEBUG("Mode: ");
      DEBUGLN(currentMode);
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
    ui.updateBool("power", powerOn);
    ui.updateInt("brightnessVal", globalBrightness);
    ui.updateInt("modeVal", currentMode);
  }
}
