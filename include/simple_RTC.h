#ifndef MY_RTC
#define MY_RTC

#include "esp_err.h"

esp_err_t syncTime(void);
int getTime(void);

#endif