#ifndef _DATALINK_HPP
#define _DATALINK_HPP

#include "esp_log.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include <cstdint>
#include <functional>

static const char *TAG = "DATALINK";

class Datalink {
public:
  enum class MessageType : uint8_t {
    Telemetry = 0x01,
    ConfigUpdate = 0x02,
    HandshakeReq = 0x03,
    HandshakeAck = 0x04,
    Ack = 0x05
  };

  struct __attribute__((packed)) EcuTelemetryData {
    uint8_t fault_codes[10];
    uint8_t injector_pulse;
    uint8_t idle_actuator;
    uint8_t o2_integra;
    uint8_t blm_02_num;
    uint8_t blm_02_counter;
    uint16_t speed;
    float throttle_position;
    float coolant_temperature_mv;
    uint8_t map_sensor;
    float lambda_sensor;
    float battery_voltage;
    float intake_air_sensor_mv;
    uint8_t flags_31;
    uint8_t flags_32;
    float ignition_advance;
    uint16_t engine_RPM;
    float coolant_temperature;
    uint8_t intake_temperature;
    uint8_t atmospheric_pressure;
    uint32_t timestamp_ms;
  };

  struct __attribute__((packed)) CarConfiguration {
    uint8_t logging_interval_ms;
    uint8_t launch_control_enabled;
    uint16_t target_rpm_limit;
    float fuel_trim_factor;
  };

  struct __attribute__((packed)) HandshakePayload {
    uint32_t magic_token;
    uint8_t device_id;
  };

  struct __attribute__((packed)) EspNowPacket {
    MessageType type;
    union {
      EcuTelemetryData telemetry;
      CarConfiguration config;
      HandshakePayload handshake;
    } payload;
  };

  using TelemetryCallback = std::function<void(
      const uint8_t *mac_addr, const EcuTelemetryData &telemetry)>;
  using ConfigCallback = std::function<void(
      const uint8_t *mac_addr, const CarConfiguration &config)>;
  using HandshakeCallback = std::function<void(
      const uint8_t *mac_addr, const HandshakePayload &handshake)>;
  using SendStatusCallback = std::function<void(const uint8_t *mac_addr,
                                                esp_now_send_status_t status)>;

  // Singleton Instance Access
  static Datalink *get_instance();

  // Lifecycle
  Datalink();
  ~Datalink();

  bool init();
  void deactivate();

  // Security & Pairing
  bool set_primary_master_key(const uint8_t *pmk);
  bool add_secure_peer(const uint8_t *peer_mac, const uint8_t *lmk,
                       uint8_t channel = 0,
                       wifi_interface_t iface = WIFI_IF_STA);
  bool send_handshake_request(const uint8_t *dest_mac, uint32_t token,
                              uint8_t dev_id);
  bool send_handshake_ack(const uint8_t *dest_mac, uint32_t token,
                          uint8_t dev_id);

  // Communication
  bool send_telemetry(const uint8_t* dest_mac, const EcuTelemetryData& telemetry);
  bool send_config(const uint8_t *dest_mac, const CarConfiguration &config);

  // Callbacks Registration
  void register_telemetry_callback(TelemetryCallback cb);
  void register_config_callback(ConfigCallback cb);
  void register_handshake_callback(HandshakeCallback cb);
  void register_send_status_callback(SendStatusCallback cb);

private:
  inline static Datalink *s_instance = nullptr;

  TelemetryCallback m_telemetry_cb = nullptr;
  ConfigCallback m_config_cb = nullptr;
  HandshakeCallback m_handshake_cb = nullptr;
  SendStatusCallback m_send_status_cb = nullptr;

  // Updated C-to-C++ Callback Bridge for modern ESP-IDF
  static void esp_now_send_callback_proxy(const esp_now_send_info_t *tx_info,
                                          esp_now_send_status_t status);
  static void esp_now_recv_callback_proxy(const esp_now_recv_info_t *recv_info,
                                          const uint8_t *data, int len);
};

#endif