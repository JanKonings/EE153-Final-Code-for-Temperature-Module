typedef struct {
    int time; // Unix Time
    float temp; // Temperature in Celsius
} measurement;

typedef enum {
    GOOD = 0,
    MEH = 1,
    BAD = 2
} Status;

void sendMessage(measurement [], int);
