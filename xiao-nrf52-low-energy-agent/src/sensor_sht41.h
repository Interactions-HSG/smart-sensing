#ifndef SHT41_h_
#define SHT41_h_

struct sht4x_reading{
    float temperature;
    float humidity;
};

bool init_shtx();
sht4x_reading read_shtx();
void reset_shtx();

#endif