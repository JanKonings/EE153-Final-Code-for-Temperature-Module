/*
Modules

storage

temp sensor

mqtt get this working

errors

*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h" // Used for timer delay
#include "esp_adc/adc_oneshot.h"
#include "soc/gpio_reg.h"
#include "driver/gpio.h" // GPIO pin controls
#include "driver/spi_master.h"

#include "nvs_flash.h"
#include "esp_netif.h"
#include "mqtt_client.h"
#include "wifi.h"
#include "spi.h"
#include "esp_log.h"
#include "esp_sleep.h"

#include "esp_timer.h"

#define WIFI_SSID      "Tufts_Wireless"
#define WIFI_PASS      ""

#define BROKER_URI "mqtt://bell-mqtt.eecs.tufts.edu"

#define POWER_GPIO 2

void app_main() {

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    gpio_reset_pin(POWER_GPIO);
    gpio_set_direction(POWER_GPIO, GPIO_MODE_OUTPUT);
    
    REG_WRITE(GPIO_OUT_W1TS_REG, 1 << POWER_GPIO);

    vTaskDelay(100/portTICK_PERIOD_MS);
    
   
    // TODO: READ TEMP HERE

    char ic_buffer[7];
    sprintf(ic_buffer, "%2.2f", ic_temp);
    // wifi
    wifi_connect(WIFI_SSID, WIFI_PASS);
    
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = BROKER_URI,
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
    esp_mqtt_client_publish(client, "phicke03/iteration1/ic_temp", ic_buffer, 0, 0, 0);
    vTaskDelay(500/portTICK_PERIOD_MS);
    esp_deep_sleep(1000*1000 * 59 * 60); //sleep for 59 minutes
}
