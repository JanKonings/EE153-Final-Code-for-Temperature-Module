
#include "tempSensor.h"

float tmp1075_read_temp(void) {
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

// Initialize I2C
void i2c_master_init(void) {
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
    return; 
  }  
  ESP_LOGI("i2c3", "Got passed first error check");

  ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_PORT, conf.mode, 0, 0, 0));
}