/*
Modules

storage

temp sensor

mqtt get this working

errors

*/


#include <stdio.h>
#include "math.h"

#include "driver/i2c.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h" // Used for timer delay
#include "esp_adc/adc_oneshot.h"
#include "soc/gpio_reg.h"
#include "driver/gpio.h" // GPIO pin controls

#include "nvs_flash.h"
#include "esp_netif.h"
#include "mqtt_client.h"
#include "minimal_wifi.h"
#include "esp_log.h"
#include "esp_sleep.h"

#include "esp_timer.h"

#define WIFI_SSID      "Tufts_Wireless"
#define WIFI_PASS      ""

#define BROKER_URI "mqtt://bell-mqtt.eecs.tufts.edu"

#define POWER_GPIO GPIO_NUM_0

// Wi-Fi and MQTT 
#define TAG "THERM+MQTT"

// ADC (Thermistor) 
#define LED_PIN    GPIO_NUM_4

// I²C (TMP1075) 
#define I2C_MASTER_PORT     I2C_NUM_0
#define I2C_MASTER_FREQ_HZ  100000
#define SDA_PIN             2
#define SCL_PIN             3
#define TMP1075_ADDR        0x48      
#define TMP1075_TEMP_REG    0x00
#define TMP1075_RESOLUTION  0.0625f  



// Initialize I2C
static void i2c_master_init(void) {
    ESP_LOGI("i2c", "Got to Init function");

  i2c_config_t conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = SDA_PIN,
    .scl_io_num = SCL_PIN,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = I2C_MASTER_FREQ_HZ,
  };

  ESP_LOGI("i2c2", "Got passed config");

  esp_err_t err = i2c_param_config(I2C_MASTER_PORT, &conf);
  ESP_LOGI("i2c3", "i2c_param_config returned: %s", esp_err_to_name(err));
  if (err != ESP_OK) {
    ESP_LOGE("i2c3", "i2c_param_config FAILED, not installing driver");
    return; // bail out instead of abort()
  }  
  ESP_LOGI("i2c3", "Got passed first error check");

  ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_PORT, conf.mode, 0, 0, 0));
}


// Read temperature (°C) from TMP1075
static float tmp1075_read_temp(void) {
  uint8_t reg = TMP1075_TEMP_REG;
  uint8_t buf[2];

  esp_err_t err = i2c_master_write_read_device(
    I2C_MASTER_PORT,
    TMP1075_ADDR,
    &reg, 1,
    buf, 2,
    pdMS_TO_TICKS(100)
  );

  if (err != ESP_OK) {
    ESP_LOGE(TAG, "TMP1075 read failed: %s", esp_err_to_name(err));
    return NAN;
  }

  int16_t raw = ((int16_t)buf[0] << 8) | buf[1];
  raw >>= 4;                       
  if (raw & 0x800) raw |= 0xF000;  
  return raw * TMP1075_RESOLUTION;
}


void app_main() {
  // I2C
  gpio_reset_pin(POWER_GPIO);
  gpio_set_direction(POWER_GPIO, GPIO_MODE_OUTPUT);
  // REG_WRITE(GPIO_OUT_W1TS_REG, 1 << POWER_GPIO);
  gpio_set_level(POWER_GPIO, 1);
  vTaskDelay(100/portTICK_PERIOD_MS);



  i2c_master_init();

  //MQTT
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

 
  

  
  
  // READ TEMP 
  // TMP1075
  float t_ic = tmp1075_read_temp();
  char msg_ic[32];
  snprintf(msg_ic, sizeof(msg_ic), "%.2f", t_ic);


  if (!isnan(t_ic)) {
    ESP_LOGI(TAG, "TMP1075: %.2f °C", t_ic);
  } else {
    ESP_LOGW(TAG, "TMP1075 reading is NaN");
  }

  // serial test
  // char ic_buffer[7];
  
  ESP_LOGI("MAIN", "Hello!");

  vTaskDelay(100/portTICK_PERIOD_MS);

  // while (1) {
  //   vTaskDelay(pdMS_TO_TICKS(1000));
  // }


  // wifi/mqtt
  wifi_connect(WIFI_SSID, WIFI_PASS);
  
  esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.uri = BROKER_URI,
  };
  esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
  esp_mqtt_client_start(client);
  esp_mqtt_client_publish(client, "jkonin01/iteration1/ic_temp", msg_ic, 0, 0, 0);
  vTaskDelay(500/portTICK_PERIOD_MS);
  // esp_deep_sleep(1000*1000 * 59 * 60); //sleep for 59 minutes
  esp_deep_sleep(1000*1000 * 1 * 3); //sleep for 59 minutes
}
