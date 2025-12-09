#include <string.h>

#include "esp_netif.h"
#include "mqtt_client.h"

#include "minimal_wifi.h"

#include "RTC.h"

#include <esp_log.h>

// #define WIFI_SSID      "Verizon_G4PLJC"
// #define WIFI_PASS      "cue9-pray-awe"

#define WIFI_SSID      "Tufts_Wireless"
#define WIFI_PASS      ""


#define BROKER_URI "mqtt://bell-mqtt.eecs.tufts.edu"
#define MQTT_TOPIC "phicke03/discard"

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
