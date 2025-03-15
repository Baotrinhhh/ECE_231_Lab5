CC = gcc
CFLAGS =-lpthread -I.
DEPS = pin_config_lib.h
bbb_int: configure_gpio.c interrupt_capture.c $(DEPS)
	$(CC) -o bbb_int configure_gpio.c interrupt_capture.c $(CFLAGS)
.PHONY: clean
clean:
	rm -f bbb_int
