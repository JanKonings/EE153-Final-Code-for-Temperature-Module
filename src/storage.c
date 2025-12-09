#include "storage.h"

measurement readings[BATCH_NUMBER];

void saveArrayData(void)
{
    nvs_handle_t DATA;
    esp_err_t err;

    // Open NVS handle
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &DATA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG2, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return;
    }

    // Write blob
    err = nvs_set_blob(DATA, "measurements", &readings, sizeof(readings));
    if (err != ESP_OK) {
        ESP_LOGE(TAG2, "Failed to write temperature/time dats array");
        nvs_close(DATA);
        return;
    }

    // Commit
    err = nvs_commit(DATA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG2, "Failed to commit data");
    }

    nvs_close(DATA);
}

void readArrayData(void)
{
    nvs_handle_t DATA;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &DATA);

    ESP_LOGI(TAG2, "Reading temperature/time pair array");
    size_t data_size = sizeof(readings);
    err = nvs_get_blob(DATA, "measurements", &readings, &data_size);

    nvs_close(DATA);
}

void addNewMeasurment(int time, float temp) {
    nvs_handle_t DATA;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &DATA);

    ESP_LOGI(TAG2, "Reading iteration number");
    int32_t iterationNum = 0;
    err = nvs_get_i32(DATA, "iterationNum", &iterationNum);

    measurement newMeasurement;
    newMeasurement.temp = temp;
    newMeasurement.time = time;

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI("debug", "INSIDE NVS NOT FOUND FOR ITERATOR");
        iterationNum = 0;
        readings[iterationNum] = newMeasurement;
        saveArrayData(); // no read because the array should be empty
        iterationNum++;

    } else if (iterationNum >= 23) {
        ESP_LOGI("debug", "INSIDE BATCH SEND");
        readArrayData();
        readings[iterationNum] = newMeasurement;
        saveArrayData();

        sendMessage(readings, iterationNum + 1);

        //TODO: add way to detect wether wifif connect/dat sent or not
        readArrayData();
        memset(readings, 0, sizeof(readings));
        saveArrayData();

        iterationNum = 0;

        vTaskDelay(10000/portTICK_PERIOD_MS);
    } else {
        ESP_LOGI("debug", "INSIDE ELSE");

        readArrayData();

        readings[iterationNum] = newMeasurement;

        for (int i = 0; i <= iterationNum; i++) {
            ESP_LOGI("DATA", "Temp: %.2f, Time: %d", readings[i].temp, readings[i].time);
        }
        saveArrayData();
        iterationNum++;
    }


    ESP_LOGI(TAG2, "writing new iteration: %d", iterationNum);

    err = nvs_set_i32(DATA, "iterationNum", iterationNum);
    if (err != ESP_OK) {
        ESP_LOGE(TAG2, "failed updating interator");
    }

    err = nvs_commit(DATA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG2, "failed committing iterator");
    }

    nvs_close(DATA);
}



