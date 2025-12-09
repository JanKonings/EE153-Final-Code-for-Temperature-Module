/*
Modules

storage

temp sensor

mqtt get this working

errors

*/


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h" // Used for timer delay
#include "soc/gpio_reg.h"
#include "driver/gpio.h" // GPIO pin controls

#include "nvs_flash.h"
#include "esp_netif.h"
#include "mqtt_client.h"
#include "minimal_wifi.h"
#include "esp_sleep.h"

#include "esp_timer.h"

#include "tempSensor.h"


#define WIFI_SSID      "Tufts_Wireless"
#define WIFI_PASS      ""

#define BROKER_URI "mqtt://bell-mqtt.eecs.tufts.edu"

#define POWER_GPIO GPIO_NUM_0

void app_main() {
  //enable temp sensor throuhg power gpio
  gpio_reset_pin(POWER_GPIO);
  gpio_set_direction(POWER_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_level(POWER_GPIO, 1);

  // initiate the i2c communication for Temp sensor
  i2c_master_init();

  // READ TEMP 
  float t_ic = tmp1075_read_temp();
  char msg_ic[32];
  snprintf(msg_ic, sizeof(msg_ic), "%.2f", t_ic);


  //flash init setup
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // COnnect to wifi
  wifi_connect(WIFI_SSID, WIFI_PASS);

  esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.uri = BROKER_URI,
  };
  esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);

  // start mqtt client
  esp_mqtt_client_start(client);

  // send data to mqtt
  esp_mqtt_client_publish(client, "jkonin01/iteration1/ic_temp", msg_ic, 0, 0, 0);
  vTaskDelay(500/portTICK_PERIOD_MS);
  esp_deep_sleep(1000*1000 * 1 * 5); //sleep for 5 seconds
}
