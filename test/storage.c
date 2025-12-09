#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"

typedef struct {
    uint32_t time;
    float temp;
} measurment;

#define BATCH_NUMBER 48

#define STORAGE_NAMESPACE "storage"

#define TAG "tempReads"

measurment readings[BATCH_NUMBER];

esp_err_t saveTempRead(measurment newMeasurment)
{
    nvs_handle_t iterationNum;
    esp_err_t err;

    nvs_handle_t tempData;
    esp_err_t err;

    // Open NVS handle
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &tempData);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return err;
    }

    // Write blob
    err = nvs_set_blob(tempData, "measurments", &readings, sizeof(readings));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write temperature/time dats array");
        nvs_close(tempData);
        return err;
    }

    // Commit
    err = nvs_commit(tempData);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit data");
    }

    nvs_close(tempData);
    return err;
}

esp_err_t read_stored_blobs(void)
{
    nvs_handle_t tempData;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &tempData);
    if (err != ESP_OK) return err;

    // 1. Read test data blob
    ESP_LOGI(TAG, "Reading temperature/time pair array");
    size_t data_size = sizeof(readings);
    err = nvs_get_blob(tempData, "measurments", &readings, &data_size);

    nvs_close(tempData);
    return ESP_OK;
}










void storeData {float temp, int time} {

}

