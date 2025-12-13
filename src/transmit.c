#include "transmit.h"
#include "RTC.h"
#include "esp_wifi.h"

static char payload[2048];

static const esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.uri = BROKER_URI,
};

// for later: Status health, int missed, int rssi, char log[], int logLength
esp_err_t sendMessage(measurement arr[], int length) {
    payload[0] = '\0';
    strlcpy(payload, "{\"measurements\":[", sizeof(payload));

    for(int i = 0; i < length; i++) {
        char pair[64];
        snprintf(pair, sizeof(pair), "[%d,%.2f%s",
                    arr[i].time, arr[i].temp, (i == (length - 1)) ? "]" : "],");
        strlcat(payload, pair, sizeof(payload));
    }

    int timeNow = getTime();
    char time_str[32];
    snprintf(time_str, sizeof(time_str), "], \"board_time\": %d, ", timeNow);
    strlcat(payload, time_str, sizeof(payload));

    char heartbeat[] = "\"heartbeat\": {\"status\": 0, ";
    strlcat(payload, heartbeat, sizeof(payload));

    // try to COnnect to wifi
    esp_err_t err1;

    if (wifi_is_connected()) {
        ESP_LOGI("WIFI", "araready connected, skipping wifif connect");
        err1 = ESP_OK;   
    } else {
        err1 = wifi_connect(WIFI_SSID, WIFI_PASS);
    }

    // record/update failure count
    int32_t wifiFails = wifiStatus(err1);


    char fail_str[32];
    snprintf(fail_str, sizeof(fail_str), "\"connections_missed\": %ld, ", wifiFails);
    strlcat(payload, fail_str, sizeof(payload));


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

    char RSSI_str[16];
    snprintf(RSSI_str, sizeof(RSSI_str), "\"rssi\": %d", RSSI);
    strlcat(payload, RSSI_str, sizeof(payload));

    strlcat(payload, "}}", sizeof(payload));

    ESP_LOGI("PAYLOAD", "%s", payload);



    // sync time and check if worked
    // esp_err_t err = syncTime();

    // if (err != ESP_OK) {
    //    ESP_LOGE("NTP", "RTC calbration failed in transmit");
    // }
    
    // esp_mqtt_client_config_t mqtt_cfg = {
    //     .broker.address.uri = BROKER_URI,
    // };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);

    // start mqtt client
    esp_mqtt_client_start(client);

    // send data to mqtt
    esp_mqtt_client_publish(client, MQTT_TOPIC, payload, 0, 0, 0);

    // char temperature[16];
    // snprintf(temperature, sizeof(temperature), "%.2f", arr[0].temp);
    // esp_mqtt_client_publish(client,"jkonin01/iteration1/ic_temp" , temperature, 0, 0, 0);


    return ESP_OK;
}

int32_t wifiStatus(esp_err_t wifiErr) {
    nvs_handle_t DATA;
    esp_err_t err1;

    int32_t wifiFails = 0;

    err1 = nvs_open("storage", NVS_READWRITE, &DATA);
    if (err1 != ESP_OK) {
        ESP_LOGE("NVS", "couldnt open nvs storage for wifi status");
        return 0; 
    }

    if (wifiErr == ESP_OK) {
        err1 = nvs_set_i32(DATA, "wifiFails", 0);
        wifiFails = 0;
    } else {
        err1 = nvs_get_i32(DATA, "wifiFails", &wifiFails);
        wifiFails += 1;
        err1 = nvs_set_i32(DATA, "wifiFails", wifiFails);
    }

    err1 = nvs_commit(DATA);

    nvs_close(DATA);

    return wifiFails;
}