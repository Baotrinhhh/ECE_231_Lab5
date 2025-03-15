#include "pin_config_lib.h"

//Sets up gpio pin as input
void configure_gpio_input(int gpio_number, char* pin_number){
    // configure the pin in GPIO mode
    config_pin(pin_number, "gpio_pu");

    // setting gpio direction as input
    char GPIODirection[40];
    sprintf(GPIODirection, "/sys/class/gpio/gpio%d/direction",
    gpio_number);

    // setting GPIO as input
    FILE* fp = fopen(GPIODirection, "w");
    fwrite("in", sizeof(char), 2, fp);

    fclose(fp);
}

// sets up an interrupt on the given GPIO pin
void configure_interrupt(int gpio_number, char* pin_number){
    // set gpio as input
    configure_gpio_input(gpio_number, pin_number);

    // configure interrupts using edge file
    char InterruptEdge[40];
    sprintf(InterruptEdge, "/sys/class/gpio/gpio%d/edge", gpio_number);

    // configure interrupt on falling edge
    FILE* fp = fopen(InterruptEdge, "w");
    fwrite("rising", sizeof(char), 7, fp);
    // configure interrupt on both edges
    //fwrite("both", sizeof(char), 4, fp);

    fclose(fp);
}

// Sets up a pin to a given mode (e.g. PWM)
void config_pin(char* pin_number, char* mode){
    // creates environment to execute shell command
    if(!vfork()){
        // execute shell command for pin configuration
        int ret = execl("/usr/bin/config-pin", "config-pin", pin_number,
        mode, NULL);
        
        if (ret < 0){
            printf("Failed to configure pin in PWM mode.\n");
            exit(-1);
        }
    }
}
