void buildPortal() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP.TITLE("PlayStation Logo Lamp");
  GP.LABEL("Управление устройством");
  GP.HR();

  GP.UPDATE("power,brightness,mode,wifi_status");

  GP.LABEL("Питание");
  GP.SWITCH("power", powerOn);
  GP.BREAK();

  GP.LABEL("Яркость");
  GP.SLIDER("brightness", globalBrightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS, 1);
  GP.BREAK();

  GP.LABEL("Режим");
  GP.SELECT("mode", "0,1,2,3,4,5,6,7,8,9,10,11", currentMode);
  GP.BREAK();

  GP.BUTTON("prev", "- Яркость");
  GP.BUTTON("next", "+ Яркость");
  GP.HR();

  GP.LABEL("WiFi");
  GP.TEXT("wifi_ssid", "SSID", data.wifiSsid);
  GP.PASS_EYE("wifi_pass", "Password", data.wifiPass);
  GP.BUTTON("wifi_save", "Сохранить WiFi");
  GP.BREAK();

  if (WiFi.status() == WL_CONNECTED) {
    String info = "Подключено: ";
    info += WiFi.SSID();
    info += " (";
    info += WiFi.localIP().toString();
    info += ")";
    GP.LABEL(info);
  } else {
    GP.LABEL("WiFi не подключен. Доступна точка AP: PS_LOGO_LAMP / 12345678");
  }

  GP.BREAK();
  GP.LABEL("OTA: http://<IP_устройства>:8080");
  GP.BUILD_END();
}
