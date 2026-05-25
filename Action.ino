void actionPortal() {
  if (portal.click()) {
    if (portal.clickBool("power", powerOn)) {
      changePowerState(powerOn);
    }

    if (portal.clickInt("brightness", globalBrightness)) {
      globalBrightness = constrain(globalBrightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
      data.globalBrightness = globalBrightness;
      memory.update();
      if (powerOn) {
        initModeState(currentMode == 11 ? autoModeCurrent : currentMode);
      }
    }

    if (portal.clickInt("mode", currentMode)) {
      currentMode = constrain(currentMode, 0, 11);
      initModeState(currentMode);
      data.currentMode = currentMode;
      memory.update();
    }
  }

  if (portal.update("powerState")) portal.answer(powerOn ? "ON" : "OFF");
  if (portal.update("brightnessState")) portal.answer((int)globalBrightness);
  if (portal.update("modeState")) portal.answer((int)currentMode);
}
