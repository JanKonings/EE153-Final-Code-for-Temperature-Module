#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h" // Used for timer delay
#include "soc/gpio_reg.h"
#include "driver/gpio.h" // GPIO pin controls

#include "nvs_flash.h"

#include "esp_sleep.h"

#include "esp_timer.h"

#include "tempSensor.h"

#include "storage.h"

#include "RTC.h"


#define POWER_GPIO GPIO_NUM_0

// function to check if the RTC has been set yet, so that even the firt measruments have an accurate time.
void RTCcheck() {
  time_t now;
  time(&now);

  if (now < 1700000000) {  
    // RTC is uninitialized
    ESP_LOGW("TIME", "RTC not calibrated, syncing now");

    esp_err_t wifi_err = wifi_connect(WIFI_SSID, WIFI_PASS);

    if (syncTime() == ESP_OK) {
      ESP_LOGI("TIME", "RTC synced successfully");
    } else {
      ESP_LOGE("TIME", "NTP sync failed!");
    }

    // if (wifi_err == ESP_OK) {
    //   esp_wifi_stop();  // save power by turnign the wifif back off
    // }

  } else {
    ESP_LOGI("TIME", "RTC already valid");
  }
}

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

  RTCcheck();
  int time = getTime();
  addNewMeasurment(time, t_ic);

  // ESP_ERROR_CHECK(nvs_flash_erase());
  // ESP_LOGI("EARSE", "earsed flash");


  // measurement msmnts[1] = {{1,1}};
  // sendMessage(msmnts, 1);
  
  vTaskDelay(500/portTICK_PERIOD_MS);
  esp_deep_sleep(1000*1000 * 15 * 60); //sleep for 15 seconds
}
