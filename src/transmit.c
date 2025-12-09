#include "transmit.h"
#include <string.h>
// for later: Status health, int missed, int rssi, char log[], int logLength
void sendMessage(Measurement arr[], int length) {
    char payload[1024] = "{\"measurements\":[";
    for(int i = 0; i < length; i++) {
        char pair[20];
        snprintf(pair, sizeof(pair), "[%d,%.2f%s",
                    arr[i].time, arr[i].temp, (i == (length - 1)) ? "]" : "],");
        strncat(payload, pair);
    }   
    strncat(payload, "]}");

    
}