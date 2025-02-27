#ifndef DS18B_h_
#define DS18B_h_

struct ds18b_reading{
    unsigned int address;
    float temperature;
};

bool init_ds18_bus();
ds18b_reading* read_ds18b_bus();

#endif