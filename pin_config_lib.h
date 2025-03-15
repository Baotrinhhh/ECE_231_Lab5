#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/epoll.h>
#include <time.h>

//Sets up gpio pin as input
void configure_gpio_input(int gpio_number, char* pin_number);
// sets up an interrupt on the gpio pin
void configure_interrupt(int gpio_number, char* pin_number);
// Sets up a pin to a given mode (e.g. PWM)
void config_pin(char* pin_number, char* mode);