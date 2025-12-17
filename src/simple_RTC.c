#include "simple_RTC.h"
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "esp_netif_sntp.h"
#include "lwip/ip_addr.h"
#include "esp_sntp.h"

static const char *TAG = "nodeRTC";
esp_err_t syncTime(void) {
    // default sntp config
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);


    // currently is waiting for five seconds, but since this is called only once at startup should be fine for power consumptino
    esp_err_t err = esp_netif_sntp_sync_wait(pdMS_TO_TICKS(5000));
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "did not sync within 5 seconds");
        return err;
    }

    ESP_LOGI(TAG, "RTC time calibradte");
    return ESP_OK;
}

int getTime(void) {
    time_t now;
    time(&now);

    // this is for tetsing the code/debugging, it prints human readble time

    // struct tm timeinfo;
    // localtime_r(&now, &timeinfo);

    // char buf[64];
    // strftime(buf, sizeof(buf), "%c", &timeinfo);
    // ESP_LOGI("TIME", "Current time: %s", buf);

    return (int)now;
}