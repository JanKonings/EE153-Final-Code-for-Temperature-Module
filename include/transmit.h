#ifndef TRANSMIT
#define TRANSMIT
#include "esp_err.h"
#define WIFI_SSID      "Tufts_Wireless"
#define WIFI_PASS      ""

typedef struct {
    int time; // Unix Time
    float temp; // Temperature in Celsius
} measurement;

typedef enum {
    GOOD = 0,
    MEH = 1,
    BAD = 2
} Status;

esp_err_t sendMessage(measurement [], int);
int32_t wifiStatus(esp_err_t err);

#endif
