#ifndef TEMP_SENSE
#define TEMP_SENSE

#define I2C_MASTER_PORT     I2C_NUM_0
#define I2C_MASTER_FREQ_HZ  100000
#define SDA_PIN             2
#define SCL_PIN             3
#define TMP1075_ADDR        0x48      
#define TMP1075_TEMP_REG    0x00
#define TMP1075_RESOLUTION  0.0625f  


float tmp1075_read_temp(void);
void i2c_master_init(void);



#endif


