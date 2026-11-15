#include "telemetry_store.hpp"

TelemetryStore::TelemetryStore() {
    m_mutex = xSemaphoreCreateMutex();
}

TelemetryStore::~TelemetryStore() {
    if (m_mutex) {
        vSemaphoreDelete(m_mutex);
    }
}

void TelemetryStore::update(const Datalink::EcuTelemetryData &data) {
    if (m_mutex && xSemaphoreTake(m_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        m_current_data = data;
        m_is_updated = true;
        xSemaphoreGive(m_mutex);
    }
}

bool TelemetryStore::get(Datalink::EcuTelemetryData &out_data) {
    bool status = false;
    if (m_mutex && xSemaphoreTake(m_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        out_data = m_current_data;
        status = m_is_updated;
        xSemaphoreGive(m_mutex);
    }
    return status;
}