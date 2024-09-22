#ifndef PLAYER_h_
#define PLAYER_h_
#include <Arduino.h>

void get_roles();
void join_role(char* role_name);
void leave_role(char* role_name);
void on_receive(uint16_t from, uint8_t key, uint32_t value);
void tell(uint16_t destination, uint8_t key, uint32_t value);

#endif /* PLAYER_h_ */