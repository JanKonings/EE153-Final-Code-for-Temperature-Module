#include <inttypes.h>

#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "transmit.h"


#include "freertos/FreeRTOS.h"

#define BATCH_NUMBER 48

#define STORAGE_NAMESPACE "storage"

#define TAG2 "tempReads"

void saveArrayData(void);
void readArrayData(void);
void addNewMeasurment(int time, float temp);

