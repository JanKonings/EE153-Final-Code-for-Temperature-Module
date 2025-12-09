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

#include "esp_sleep.h"

#include "esp_timer.h"

#include "tempSensor.h"

// #include "transmit.h"

#include "storage.h"


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
        ESP_ERROR_CHECK(nvs_flash_erase());//maybe be careful this doesn't whipe memory
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    int time = 10000;
    addNewMeasurment(time, t_ic);

    // ESP_ERROR_CHECK(nvs_flash_erase());
    // ESP_LOGI("EARSE", "earsed flash");


    // measurement msmnts[1] = {{1,1}};

    // sendMessage(msmnts, 1);
  
  vTaskDelay(500/portTICK_PERIOD_MS);
  esp_deep_sleep(1000*1000 * 1 * 1); //sleep for 10 seconds
}
