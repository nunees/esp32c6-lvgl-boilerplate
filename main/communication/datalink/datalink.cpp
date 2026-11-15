#include "datalink.hpp"
#include <cstring>
#include "esp_log.h"

Datalink *Datalink::get_instance()
{
    return s_instance;
}

Datalink::Datalink()
{
    s_instance = this;
}

Datalink::~Datalink()
{
    deactivate();
}

bool Datalink::init(wifi_mode_t mode)
{
    ESP_LOGI(TAG, "Initializing Secure ESP-NOW Car Comm...");

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(mode));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));

    if (esp_now_init() != ESP_OK)
    {
        ESP_LOGE(TAG, "ESP-NOW initialization failed");
        return false;
    }

    ESP_ERROR_CHECK(esp_now_register_send_cb(esp_now_send_callback_proxy));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(esp_now_recv_callback_proxy));

    ESP_LOGI(TAG, "Secure ESP-NOW Initialized Successfully");
    return true;
}

void Datalink::deactivate()
{
    esp_now_deinit();
    s_instance = nullptr;
}

bool Datalink::set_primary_master_key(const uint8_t *pmk)
{
    esp_err_t err = esp_now_set_pmk(pmk);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set PMK: %d", err);
        return false;
    }
    ESP_LOGI(TAG, "PMK set successfully");
    return true;
}

bool Datalink::add_secure_peer(const uint8_t *peer_mac, const uint8_t *lmk, uint8_t channel, wifi_interface_t iface)
{
    if (esp_now_is_peer_exist(peer_mac))
    {
        esp_now_del_peer(peer_mac);
    }

    esp_now_peer_info_t peerInfo = {};
    std::memcpy(peerInfo.peer_addr, peer_mac, ESP_NOW_ETH_ALEN);
    peerInfo.channel = channel;
    peerInfo.ifidx = iface;
    peerInfo.encrypt = (lmk != nullptr);

    if (lmk != nullptr)
    {
        std::memcpy(peerInfo.lmk, lmk, ESP_NOW_KEY_LEN);
    }
    else
    {
        std::memset(peerInfo.lmk, 0, ESP_NOW_KEY_LEN);
    }

    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to add secure ESP-NOW peer");
        return false;
    }
    ESP_LOGI(TAG, "Secure Peer added successfully");
    return true;
}

bool Datalink::send_handshake_request(const uint8_t *dest_mac, uint32_t token, uint8_t dev_id)
{
    EspNowPacket packet = {};
    packet.type = MessageType::HandshakeReq;
    packet.payload.handshake = {token, dev_id};

    return (esp_now_send(dest_mac, reinterpret_cast<uint8_t *>(&packet), sizeof(packet)) == ESP_OK);
}

bool Datalink::send_handshake_ack(const uint8_t *dest_mac, uint32_t token, uint8_t dev_id)
{
    EspNowPacket packet = {};
    packet.type = MessageType::HandshakeAck;
    packet.payload.handshake = {token, dev_id};

    return (esp_now_send(dest_mac, reinterpret_cast<uint8_t *>(&packet), sizeof(packet)) == ESP_OK);
}

bool Datalink::send_config(const uint8_t *dest_mac, const CarConfiguration &config)
{
    EspNowPacket packet = {};
    packet.type = MessageType::ConfigUpdate;
    packet.payload.config = config;

    return (esp_now_send(dest_mac, reinterpret_cast<uint8_t *>(&packet), sizeof(packet)) == ESP_OK);
}

bool Datalink::send_telemetry(const uint8_t *dest_mac, const EcuTelemetryData &telemetry)
{
    EspNowPacket packet = {};
    packet.type = MessageType::Telemetry;
    packet.payload.telemetry = telemetry;

    return (esp_now_send(dest_mac, reinterpret_cast<uint8_t *>(&packet), sizeof(packet)) == ESP_OK);
}

void Datalink::register_telemetry_callback(TelemetryCallback cb)
{
    m_telemetry_cb = cb;
}

void Datalink::register_config_callback(ConfigCallback cb)
{
    m_config_cb = cb;
}

void Datalink::register_handshake_callback(HandshakeCallback cb)
{
    m_handshake_cb = cb;
}

void Datalink::register_send_status_callback(SendStatusCallback cb)
{
    m_send_status_cb = cb;
}

void Datalink::esp_now_send_callback_proxy(const esp_now_send_info_t *tx_info, esp_now_send_status_t status)
{
    if (s_instance && s_instance->m_send_status_cb)
    {
        s_instance->m_send_status_cb(tx_info->des_addr, status);
    }
}

void Datalink::esp_now_recv_callback_proxy(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len)
{

    ESP_LOGI(TAG, "Packet received! Len: %d from MAC: %02X:%02X:%02X:%02X:%02X:%02X",
             len,
             recv_info->src_addr[0], recv_info->src_addr[1], recv_info->src_addr[2],
             recv_info->src_addr[3], recv_info->src_addr[4], recv_info->src_addr[5]);

    if (!s_instance || len < static_cast<int>(sizeof(MessageType)))
    {
        return;
    }

    const auto *packet = reinterpret_cast<const EspNowPacket *>(data);

    if (packet->type == MessageType::Telemetry && len >= static_cast<int>(sizeof(EspNowPacket)))
    {
        if (s_instance->m_telemetry_cb)
        {
            s_instance->m_telemetry_cb(recv_info->src_addr, packet->payload.telemetry);
        }
    }
    else if (packet->type == MessageType::ConfigUpdate && len >= static_cast<int>(sizeof(MessageType) + sizeof(CarConfiguration)))
    {
        if (s_instance->m_send_status_cb || s_instance->m_config_cb)
        {
            s_instance->m_config_cb(recv_info->src_addr, packet->payload.config);
        }
    }
    else if (packet->type == MessageType::HandshakeReq || packet->type == MessageType::HandshakeAck)
    {
        if (s_instance->m_handshake_cb && len >= static_cast<int>(sizeof(MessageType) + sizeof(HandshakePayload)))
        {
            s_instance->m_handshake_cb(recv_info->src_addr, packet->payload.handshake);
        }
    }
}