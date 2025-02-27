
#include "player.h"
#include "ble_uart_peripheral.h"

void get_roles(){}

void join_role(char* role_name){}

void leave_role(char* role_name){}

void on_receive(uint16_t from, uint8_t key, uint32_t value){}

void tell(uint16_t destination, uint8_t key, uint32_t value)
{
    start_adv(key, value, 30, 60);
}