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

esp_err_t saveArrayData(void)
{
    nvs_handle_t DATA;
    esp_err_t err;

    // Open NVS handle
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &DATA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return err;
    }

    // Write blob
    err = nvs_set_blob(DATA, "measurments", &readings, sizeof(readings));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write temperature/time dats array");
        nvs_close(DATA);
        return err;
    }

    // Commit
    err = nvs_commit(DATA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit data");
    }

    nvs_close(DATA);
    return err;
}

void readArrayData(void)
{
    nvs_handle_t DATA;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &DATA);
    if (err != ESP_OK) return err;

    ESP_LOGI(TAG, "Reading temperature/time pair array");
    size_t data_size = sizeof(readings);
    err = nvs_get_blob(DATA, "measurments", &readings, &data_size);

    nvs_close(DATA);
}

void saveIterator(void) {
}


void addNewMeasurment(uint64_t time, float temp) {
    nvs_handle_t DATA;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &DATA);
    if (err != ESP_OK) return err;

    ESP_LOGI(TAG, "Reading iteration number");
    int32_t iterationNum = 0;
    err = nvs_get_i32(DATA, "iterationNum", &iterationNum);
    // switch (err) {
    //     case ESP_OK:
    //         ESP_LOGI(TAG, "Read counter = %" PRIu32, iterationNum);
    //         break;
    //     case ESP_ERR_NVS_NOT_FOUND:
    //         ESP_LOGW(TAG, "The value is not initialized yet!");
    //         break;
    //     default:
    //         ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
    // }

    measurment newMeasurement;
    newMeasurement.temp = temp;
    newMeasurement.time = time;

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        iterationNum = 0;
        readArrayData();
        readings[iterationNum] = newMeasurement;
        saveArrayData();
    } else if (iterationNum >= 23) {
        readArrayData();
        readings[iterationNum] = newMeasurement;
        saveArrayData();

        


    } else {
        readArrayData();
        readings[iterationNum] = newMeasurement;
        saveArrayData();
    }

    iterationNum++;

    ESP_LOGI(TAG, "writing nerw iteration");
    err = nvs_set_i32(DATA, "counter", iterationNum);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write counter!");
    }

     


}












void storeData {float temp, int time} {

}

