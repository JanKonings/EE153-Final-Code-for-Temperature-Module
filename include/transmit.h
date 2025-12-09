typedef struct {
    int time; // Unix Time
    float temp; // Temperature in Celsius
} Measurement;

typedef enum {
    GOOD = 0,
    MEH = 1,
    BAD = 2
} Status;

void sendMessage(Measurement [], int);
