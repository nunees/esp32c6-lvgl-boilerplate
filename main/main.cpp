#include "esp_log.h"

#include "button.hpp"
#include "screen_manager.hpp"

#include "esp_lvgl_port.h"
#include "lvgl.h"

#include "app.hpp"

static ScreenManager screen_manager;

extern "C" void app_main() {

  static App app;

  ESP_LOGI("MAIN", "Calling App::run()");

  app.run();

  if (lvgl_port_lock(0)) {
    screen_manager.init();

    lvgl_port_unlock();
  };

  ESP_ERROR_CHECK(btn_init(&screen_manager)); //
}
