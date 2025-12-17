#include "webLog.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#define LOG_BUFFER_SIZE 1024
char my_log_buffer[LOG_BUFFER_SIZE];
int buffer_pos = 0;

void saveArrayData(void)
{
    nvs_handle_t DATA;
    esp_err_t err;

    // Open NVS handle
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &DATA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return;
    }

    // Write blob
    err = nvs_set_blob(DATA, "measurements", &readings, sizeof(readings));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write temperature/time dats array");
        nvs_close(DATA);
        return;
    }

    // Commit
    err = nvs_commit(DATA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit data");
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
        ESP_LOGW(TAG, "readArrayData: nvs_open failed");
        memset(readings, 0, sizeof(readings));   
        return;
    }

    // Read blob
    ESP_LOGI(TAG, "Reading temperature/time pair array");
    size_t data_size = sizeof(readings);
    err = nvs_get_blob(DATA, "measurements", &readings, &data_size);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "readArrayData: no previous measurements, clearing array");
        memset(readings, 0, sizeof(readings));
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "readArrayData: nvs_get_blob failed");
        memset(readings, 0, sizeof(readings));
    }

    // Close NVS handle
    nvs_close(DATA);
}


int log_vprintf_like(const char* format, va_list vlist) {
    int written = vsnprintf(my_log_buffer + buffer_pos, 
                            LOG_BUFFER_SIZE - buffer_pos, format, vlist);
    
}

void init_webLog(void){
    
}

void flush_webLog(void){
    
}