void actionPortal() {
  if (portal.update("power")) {
    changePowerState((bool)portal.getCheck("power"));
  }

  if (portal.update("brightness")) {
    globalBrightness = constrain(portal.getInt("brightness"), MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    data.globalBrightness = globalBrightness;
    memory.update();
  }

  if (portal.update("mode")) {
    uint8_t selectedMode = constrain(portal.getInt("mode"), 0, 11);
    if (selectedMode != currentMode) {
      currentMode = selectedMode;
      initModeState(currentMode);
      data.currentMode = currentMode;
      memory.update();
    }
  }

  if (portal.click("prev")) changeBrightness(-STEP_MANUAL_BRIGHTNESS);
  if (portal.click("next")) changeBrightness(STEP_MANUAL_BRIGHTNESS);

  if (portal.click("wifi_save")) {
    String ssid = portal.getString("wifi_ssid");
    String pass = portal.getString("wifi_pass");

    ssid.trim();
    pass.trim();

    if (ssid.length() > 0) {
      ssid.toCharArray(data.wifiSsid, sizeof(data.wifiSsid));
      pass.toCharArray(data.wifiPass, sizeof(data.wifiPass));
      data.wifiConfigured = true;
      memory.update();

      WiFi.begin(data.wifiSsid, data.wifiPass);
      Serial.println("Trying to connect with new WiFi credentials...");
    }
  }
}
