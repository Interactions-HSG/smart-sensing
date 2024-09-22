#ifndef BMP180_h_
#define BMP180_h_

struct bmp_reading{
    float temperature;
    float humidity;
    float pressure;
};

bool init_bmp();
bmp_reading read_bmp();
void reset_bmp();

#endif