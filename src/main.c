#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "esp_err.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "driver/gpio.h"
#include <math.h> 
#include "nvs_flash.h"
#include "esp_netif.h"
#include "mqtt_client.h"
#include "minimal_wifi.h"

// MQTTT STUFF
#define WIFI_SSID      "tufts_eecs"
#define WIFI_PASS      "foundedin1883"

#define BROKER_URI "mqtt://bell-mqtt.eecs.tufts.edu"

void app_main(void)
{
    // Enable Flash (aka non-volatile storage, NVS)
    esp_err_t ret2 = nvs_flash_init();
    if (ret2 == ESP_ERR_NVS_NO_FREE_PAGES || ret2 == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret2 = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret2);

    // Normally we'd need to initialize the event loop and the network stack,
    // but both of these will be done when we connect to WiFi.
    printf("Connecting to WiFi...");
    wifi_connect(WIFI_SSID, WIFI_PASS);

    // Initialize the MQTT client
    // Read the documentation for more information on what you can configure:
    // https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-reference/protocols/mqtt.html
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = BROKER_URI,
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);


    //publish
    esp_mqtt_client_publish(client, "jkonin01/iteration1/thermistor_temp", , 0, 0, 0);

}
