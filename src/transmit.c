#include "transmit.h"
#include "RTC.h"


// for later: Status health, int missed, int rssi, char log[], int logLength
esp_err_t sendMessage(measurement arr[], int length) {
    char payload[1024] = "{\"measurements\":[";
    for(int i = 0; i < length; i++) {
        char pair[32];
        snprintf(pair, sizeof(pair), "[%d,%.2f%s",
                    arr[i].time, arr[i].temp, (i == (length - 1)) ? "]" : "],");
        strncat(payload, pair, 32);
    }
    // TODO if heartbeat != null, use heartbeat struct
    strcat(payload, "]}");

    ESP_LOGI("PAYLOAD", "%s", payload);

    // try to COnnect to wifi
    esp_err_t err1 = wifi_connect(WIFI_SSID, WIFI_PASS);

    if (err1 != ESP_OK) {
        ESP_LOGE("WIFI", "failed to connect to wifi");
        return err1;
    }

    // sync time and check if worked
    // esp_err_t err = syncTime();

    // if (err != ESP_OK) {
    //    ESP_LOGE("NTP", "RTC calbration failed in transmit");
    // }
    

    // esp_mqtt_client_config_t mqtt_cfg = {
    //     .broker.address.uri = BROKER_URI,
    // };
    // esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);

    // // start mqtt client
    // esp_mqtt_client_start(client);

    // // send data to mqtt
    // esp_mqtt_client_publish(client, MQTT_TOPIC, payload, 0, 0, 0);

    return ESP_OK;
}