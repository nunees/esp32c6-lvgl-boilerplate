#pragma once

#include "datalink.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

class TelemetryStore {
public:
  TelemetryStore();
  ~TelemetryStore();

  // Thread-safe write (e.g., from ESP-NOW callback)
  void update(const Datalink::EcuTelemetryData &data);

  // Thread-safe read (e.g., from UI refresh task)
  bool get(Datalink::EcuTelemetryData &out_data);

private:
  Datalink::EcuTelemetryData m_current_data{};
  bool m_is_updated = false;
  SemaphoreHandle_t m_mutex = nullptr;
};
