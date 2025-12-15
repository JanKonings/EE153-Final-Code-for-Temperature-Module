#include "storage.h"

// array to hold measurements in memory
measurement readings[BATCH_NUMBER];

// function to save temp/time array data to nvs
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

// function to read temp/time array data from nvs
void readArrayData(void)
{
    nvs_handle_t DATA;
    esp_err_t err;

    // Open NVS handle
    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &DATA);
    if (err != ESP_OK) {
        ESP_LOGW(TAG2, "readArrayData: nvs_open failed");
        memset(readings, 0, sizeof(readings));   
        return;
    }

    // Read blob
    ESP_LOGI(TAG2, "Reading temperature/time pair array");
    size_t data_size = sizeof(readings);
    err = nvs_get_blob(DATA, "measurements", &readings, &data_size);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG2, "readArrayData: no previous measurements, clearing array");
        memset(readings, 0, sizeof(readings));
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG2, "readArrayData: nvs_get_blob failed");
        memset(readings, 0, sizeof(readings));
    }

    // Close NVS handle
    nvs_close(DATA);
}

// function to add a new measurement to nvs storage
void addNewMeasurment(int time, float temp) {
    nvs_handle_t DATA;
    esp_err_t err;

    // check if first run
    bool first_run = false;

    // Open NVS handle
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &DATA);

    // initializing iterationNum and checking if it already exists in nvs
    ESP_LOGI(TAG2, "Reading iteration number");
    int32_t iterationNum = 0;
    err = nvs_get_i32(DATA, "iterationNum", &iterationNum);

    // if not found, first run
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI("debug", "INSIDE NVS NOT FOUND FOR ITERATOR");
        first_run = true;         
        iterationNum = 0;
    } else if (err != ESP_OK) { // other error
        ESP_LOGW(TAG2, "nvs_get_i32(iterationNum) failed, resetting to 0");
        iterationNum = 0;
    }

    // safety so we dont go out of bounds
    if (iterationNum < 0) {
        iterationNum = 0;
    } else if (iterationNum >= BATCH_NUMBER) {
        ESP_LOGW(TAG2, "iterationNum (%d) >= BATCH_NUMBER (%d), resetting to BATCH_NUMBER - 1", iterationNum, BATCH_NUMBER);
        iterationNum = BATCH_NUMBER - 1;
    }

    // create new measurement struct
    measurement newMeasurement;
    newMeasurement.temp = temp;
    newMeasurement.time = time;

    // logic to handle adding new measurement and sending batch if needed
    if (first_run) {
        readings[iterationNum] = newMeasurement;
        saveArrayData(); // no read because the array should be empty
        iterationNum = 1;

    } else if (iterationNum >= 11) { // if we have reached batch size of 12 (0-11)
        ESP_LOGI("debug", "INSIDE BATCH SEND");
        readArrayData();
        readings[iterationNum] = newMeasurement;
        saveArrayData();

        esp_err_t err1 = sendMessage(readings, iterationNum + 1);

        // if wifi connected and sent data erase all emasurments adn restart iterationNum
        if (err1 == ESP_OK) {
            memset(readings, 0, sizeof(readings));
            saveArrayData();
            iterationNum = -1;
        }
    
        iterationNum++;

        // this is just for debugging so that i can see the batc send for longer before it goes throuhg iteratiosn again
        // vTaskDelay(10000/portTICK_PERIOD_MS);
    } else {
        ESP_LOGI("debug", "INSIDE ELSE");

        readArrayData();

        readings[iterationNum] = newMeasurement;

        // for debugging, print all stored data

        // for (int i = 0; i <= iterationNum; i++) {
        //     ESP_LOGI("DATA", "Temp: %.2f, Time: %d", readings[i].temp, readings[i].time);
        // }

        saveArrayData();
        iterationNum++;
    }


    ESP_LOGI(TAG2, "writing new iteration: %d", iterationNum);

    // update iterationNum in nvs
    err = nvs_set_i32(DATA, "iterationNum", iterationNum);
    if (err != ESP_OK) {
        ESP_LOGE(TAG2, "failed updating interator");
    }

    // commit changes
    err = nvs_commit(DATA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG2, "failed committing iterator");
    }

    // close nvs
    nvs_close(DATA);
}



