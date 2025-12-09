#ifndef MINIMAL_WIFI
#define MINIMAL_WIFI
#include "esp_err.h"

//changed to return esp_err for check
esp_err_t wifi_connect(const char* ssid, const char* pass);

#endif