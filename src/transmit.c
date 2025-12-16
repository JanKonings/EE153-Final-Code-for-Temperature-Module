#include "transmit.h"
#include "RTC.h"
#include "esp_wifi.h"
#include <string.h>
#include "esp_netif.h"
#include "mqtt_client.h"
#include "minimal_wifi.h"
#include <esp_log.h>
#include "nvs.h"
#include "simple_RTC.h"

// #define WIFI_SSID      "Verizon_G4PLJC"
// #define WIFI_PASS      "cue9-pray-awe"
#define BROKER_URI "mqtt://bell-mqtt.eecs.tufts.edu"
#define MQTT_TOPIC "teamN/node0/update"
static const char *TAG = "Transmit";

// static buffer to hold payload
static char payload[2048];

// mqtt client configuration
static const esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.uri = BROKER_URI,
};

// function to send message to mqtt broker
esp_err_t sendMessage(measurement arr[], int length) {
    // construct payload
    payload[0] = '\0';
    // begin with measurements array
    strlcpy(payload, "{\"measurements\":[", sizeof(payload));

    // append each measurement pair
    for(int i = 0; i < length; i++) {
        char pair[64];
        snprintf(pair, sizeof(pair), "[%d,%.2f%s",
                    arr[i].time, arr[i].temp, (i == (length - 1)) ? "]" : "],");
        strlcat(payload, pair, sizeof(payload));
    }

    // close measurements array and add board time 
    int timeNow = getTime();
    char time_str[32];
    snprintf(time_str, sizeof(time_str), "], \"board_time\": %d, ", timeNow);
    strlcat(payload, time_str, sizeof(payload));

    // add heartbeat object
    char heartbeat[] = "\"heartbeat\": {\"status\": 0, ";
    strlcat(payload, heartbeat, sizeof(payload));

    // try to COnnect to wifi
    esp_err_t err1;

    // check if already connected (was having issues when RTC would connect and then we would try again here)
    if (wifi_is_connected()) {
        ESP_LOGI("WIFI", "araready connected, skipping wifif connect");
        err1 = ESP_OK;   
    } else {
        err1 = wifi_connect(WIFI_SSID, WIFI_PASS);
    }

    // record/update failure count
    int32_t wifiFails = wifiStatus(err1);

    // add connection_missed field to heartbeat
    char fail_str[32];
    snprintf(fail_str, sizeof(fail_str), "\"connections_missed\": %ld, ", wifiFails);
    strlcat(payload, fail_str, sizeof(payload));

    // check if wifi connection was successful
    if (err1 != ESP_OK) {
        ESP_LOGE("WIFI", "failed to connect to wifi");
        return err1;
    }

    // get RSSI
    int RSSI = 0;
    wifi_ap_record_t ap_info;
    esp_err_t err = esp_wifi_sta_get_ap_info(&ap_info);
    if (err == ESP_OK) {
        RSSI = ap_info.rssi;
    } else {
        ESP_LOGE("WIFI", "failed to egt rssi");
    }

    // add rssi field to heartbeat
    char RSSI_str[16];
    snprintf(RSSI_str, sizeof(RSSI_str), "\"rssi\": %d", RSSI);
    strlcat(payload, RSSI_str, sizeof(payload));

    strlcat(payload, "}}", sizeof(payload));

    ESP_LOGI("PAYLOAD", "%s", payload);

    // initialize mqtt client
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);

    // start mqtt client
    esp_mqtt_client_start(client);

    // send data to mqtt
    esp_mqtt_client_publish(client, MQTT_TOPIC, payload, 0, 0, 0);

    return ESP_OK;
}

// function to record wifi connection status in nvs
int32_t wifiStatus(esp_err_t wifiErr) {
    nvs_handle_t DATA;
    esp_err_t err1;

    int32_t wifiFails = 0;

    // open nvs
    err1 = nvs_open("storage", NVS_READWRITE, &DATA);
    if (err1 != ESP_OK) {
        ESP_LOGE("NVS", "couldnt open nvs storage for wifi status");
        return 0; 
    }

    // update wifiFails count
    if (wifiErr == ESP_OK) {
        err1 = nvs_set_i32(DATA, "wifiFails", 0);
        wifiFails = 0;
    } else {
        err1 = nvs_get_i32(DATA, "wifiFails", &wifiFails);
        wifiFails += 1;
        err1 = nvs_set_i32(DATA, "wifiFails", wifiFails);
    }

    // commit changes
    err1 = nvs_commit(DATA);

    // close nvs
    nvs_close(DATA);

    return wifiFails;
}