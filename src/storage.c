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
        // return err;
        return;
    }

    // Write blob
    err = nvs_set_blob(DATA, "measurements", &readings, sizeof(readings));
    if (err != ESP_OK) {
        ESP_LOGE(TAG2, "Failed to write temperature/time dats array");
        nvs_close(DATA);
        // return err;
        return;
    }

    // Commit
    err = nvs_commit(DATA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG2, "Failed to commit data");
    }

    nvs_close(DATA);
    // return err;
}

void readArrayData(void)
{
    nvs_handle_t DATA;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &DATA);
    // if (err != ESP_OK) return err;

    ESP_LOGI(TAG2, "Reading temperature/time pair array");
    size_t data_size = sizeof(readings);
    err = nvs_get_blob(DATA, "measurements", &readings, &data_size);

    nvs_close(DATA);
}

void addNewMeasurment(int time, float temp) {
    nvs_handle_t DATA;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &DATA);
    // if (err != ESP_OK) return err;

    ESP_LOGI(TAG2, "Reading iteration number");
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

    measurement newMeasurement;
    newMeasurement.temp = temp;
    newMeasurement.time = time;

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI("debug", "INSIDE NVS NOT FOUND FOR ITERATOR");
        iterationNum = 0;
        // readArrayData();
        readings[iterationNum] = newMeasurement;
        saveArrayData();
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



