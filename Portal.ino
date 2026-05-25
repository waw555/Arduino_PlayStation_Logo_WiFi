void buildPortal() {
  GP.BUILD_BEGIN(GP_DARK);
  GP.PAGE_TITLE("PS Logo Lamp");
  GP.UPDATE("powerState,brightnessState,modeState");
  GP.ONLINE_CHECK(5000);

  M_GRID(
    M_BLOCK_TAB(
      "УПРАВЛЕНИЕ",
      M_BOX(GP.LABEL("Питание"); GP.SWITCH("power", powerOn););
      GP.BREAK();
      M_BOX(GP.LABEL("Яркость"); GP.SLIDER("brightness", globalBrightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS););
      GP.BREAK();
      M_BOX(GP.LABEL("Режим"); GP.SPINNER("mode", currentMode, 0, 11););
      GP.BREAK();
      GP.HR(GP_GRAY);
      GP.BREAK();
      M_BOX(GP.LABEL("Состояние"); GP.LABEL_BLOCK("", "powerState"););
      GP.BREAK();
      M_BOX(GP.LABEL("Текущая яркость"); GP.LABEL_BLOCK("0", "brightnessState"););
      GP.BREAK();
      M_BOX(GP.LABEL("Текущий режим"); GP.LABEL_BLOCK("0", "modeState"););
    );
  );

  GP.BUILD_END();
}
