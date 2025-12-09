#include "RTC.h"

esp_err_t syncTime() {
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);

    // if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(1000)) != ESP_OK) {
    //     ESP_LOGE("NTP", "Failed to sync time within 1 second");
    // }


    // currently is waiting for five seconds we can def make shorter to save power
    esp_err_t err = esp_netif_sntp_sync_wait(pdMS_TO_TICKS(5000));
    if (err != ESP_OK) {
        ESP_LOGW("NTP", "did not sync within 5 seconds");
        return err;
    }

    ESP_LOGI("NTP", "RTC time calibradte");
    return ESP_OK;
}

int getTime() {
    time_t now;
    time(&now);

    // this is for tetsing the code, it prints huma. readble time
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    char buf[64];
    strftime(buf, sizeof(buf), "%c", &timeinfo);
    ESP_LOGI("TIME", "Current time: %s", buf);

    return (int)now;
}