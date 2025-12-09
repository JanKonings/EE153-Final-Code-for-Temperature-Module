#include "transmit.h"
#include <string.h>

#include "esp_netif.h"
#include "mqtt_client.h"
#include "minimal_wifi.h"

#include <esp_log.h>

#define WIFI_SSID      "Tufts_Wireless"
#define WIFI_PASS      ""
#define BROKER_URI "mqtt://bell-mqtt.eecs.tufts.edu"
#define MQTT_TOPIC "phicke03/discard"


// for later: Status health, int missed, int rssi, char log[], int logLength
void sendMessage(measurement arr[], int length) {
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
    // COnnect to wifi
    // wifi_connect(WIFI_SSID, WIFI_PASS);

    // esp_mqtt_client_config_t mqtt_cfg = {
    //     .broker.address.uri = BROKER_URI,
    // };
    // esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);

    // // start mqtt client
    // esp_mqtt_client_start(client);

    // // send data to mqtt
    // esp_mqtt_client_publish(client, MQTT_TOPIC, payload, 0, 0, 0);
}