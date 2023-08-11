// Support for motors on the Pololu 3Pi+ 2040 robot.
// Based on code Copyright (C) Pololu Corporation.  See LICENSE.txt for details.

#include <stdbool.h>
#include <stdint.h>

// GPIO pin numbers.
#define RIGHT_MOTOR_DIRECTION 10
#define LEFT_MOTOR_DIRECTION 11
#define RIGHT_MOTOR_SPEED 14
#define LEFT_MOTOR_SPEED 15

// Motor power is a 12 bit number.
#define MAX_MOTOR_POWER 0x0FFF

/**
 * @brief Initialize the motors.
 * This sets up the GPIO pins and PWM controllers for the motors.
 * @param clock_divider The extent to which to divide the master clock
 * for the PWM controller.  A value of 1 with a 125MHz clock yields a
 * PWM cycle frequency of about 30.5 kHz (125,000,000/4096) for a 12-bit
 * PWM controller.
 */
void motors_init(uint8_t clock_divider);

/**
 * @brief Set the power of the left or right motor moving forward or backward.
 * @param power The power, ranging from 0 to MAX_MOTOR_POWER (clamped).
 * @param forward True to move forward, false to move backward.
 * @param left True to drive the left motor, false to drive the right motor.
 */
void motors_set_power(uint16_t power, bool forward, bool left);
