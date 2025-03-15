#!/bin/sh
cd /sys/class/pwm/pwmchip4/pwm-4\:0
echo 0 > enable
echo 'PWM Stopped'