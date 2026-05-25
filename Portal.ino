void buildPortal() {
  GP.BUILD_BEGIN(GP_DARK);

  GP.PAGE_TITLE("PlayStation Logo Lamp");
  GP.UPDATE("power,brightness,mode,wifi_status");
  GP.ONLINE_CHECK(5000);

  GP.UI_MENU("PlayStation Logo Lamp", GP_RED);
  GP.UI_LINK("/", "Состояние");
  GP.UI_LINK("/wifi", "WiFi");
  GP.UI_LINK("/ota_update", "Обновление");

  GP.HR(GP_GRAY);
  GP.LABEL("Текущее время");
  GP.BREAK();
  GP.LABEL(portal.getSystemTime().encode());

  GP.UI_BODY();
  GP.GRID_RESPONSIVE(700);

  if (portal.uri("/")) {
    M_GRID(
      M_BLOCK_TAB(
        "СОСТОЯНИЕ",
        M_BOX(GP.LABEL("Питание: "); GP.SWITCH("power", powerOn););
        GP.BREAK();
        M_BOX(GP.LABEL("Яркость: "); GP.SLIDER("brightness", globalBrightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS, 1););
        GP.BREAK();
        M_BOX(GP.LABEL("Режим: "); GP.SELECT("mode", "0,1,2,3,4,5,6,7,8,9,10,11", currentMode););
        GP.BREAK();
        M_BOX(GP.BUTTON("prev", "- Яркость"); GP.BUTTON("next", "+ Яркость"););
      );
    );
  } else if (portal.uri("/wifi")) {
    GP.FORM_BEGIN("/wifi");
    M_GRID(
      M_BLOCK_TAB(
        "ПОДКЛЮЧЕНИЕ К WIFI",
        GP.TEXT("wifi_ssid", "SSID", data.wifiSsid, "", 32);
        GP.BREAK();
        GP.PASS_EYE("wifi_pass", "Пароль", data.wifiPass, "", 64);
        GP.BREAK();
        GP.SUBMIT("Сохранить");
      );
    );
    GP.FORM_END();

    GP.BREAK();
    if (WiFi.status() == WL_CONNECTED) {
      String info = "Подключено: ";
      info += WiFi.SSID();
      info += " (";
      info += WiFi.localIP().toString();
      info += ")";
      GP.LABEL(info);
    } else {
      GP.LABEL("WiFi не подключен. AP: PS_LOGO_LAMP / 12345678");
    }
  } else if (portal.uri("/ota_update")) {
    M_GRID(
      M_BLOCK_TAB(
        "ОБНОВЛЕНИЕ",
        GP.OTA_FIRMWARE("Обновить прошивку");
        GP.BREAK();
        GP.OTA_FILESYSTEM("Обновить файловую систему");
      );
    );
  }

  GP.UI_END();
  GP.BUILD_END();
}
