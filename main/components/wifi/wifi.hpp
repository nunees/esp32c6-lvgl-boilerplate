#ifndef _WIFI_HPP
#define _WIFI_HPP

#include "esp_err.h"
#include "esp_event_base.h"
#include "esp_wifi_types_generic.h"
#include <cstdint>
#include <functional>

class Wifi {
public:
  using WifiEventCallback =
      std::function<void(esp_event_base_t base, int32_t id, void *data)>;

  Wifi();
  ~Wifi();

  // Initializes netif, default event loop (if needed), and the Wi-Fi driver
  esp_err_t init(wifi_mode_t mode = WIFI_MODE_STA, uint8_t channel = 1);
  void deinit();

  // Radio & Channel Control
  bool set_channel(uint8_t primary_channel,
                   wifi_second_chan_t second = WIFI_SECOND_CHAN_NONE);
  uint8_t get_channel() const;
  bool get_mac_address(uint8_t *mac_out,
                       wifi_interface_t iface = WIFI_IF_STA) const;

  // Extensibility for future Station/AP features
  bool connect_sta(const char *ssid, const char *password);
  bool start_softap(const char *ssid, const char *password, uint8_t channel = 1,
                    uint8_t max_conn = 4);
  void register_event_callback(WifiEventCallback cb);

private:
  static constexpr const char *TAG = "Wifi";
  bool m_initialized = false;
  WifiEventCallback m_event_cb = nullptr;

  static void wifi_event_handler_proxy(void *arg, esp_event_base_t event_base,
                                       int32_t event_id, void *event_data);
};

#endif
