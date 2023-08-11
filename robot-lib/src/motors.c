/**
 * @file motors.c
 * @author Abhi Gundrala
 * @author Edward A. Lee
 * @brief C support for motors on a Pololu 3Pi+ 2040 robot.
 * This code is based on Raspberry Pi Pico examples, specifically:
 * https://github.com/raspberrypi/pico-examples/blob/master/pwm/hello_pwm/hello_pwm.c
 */
#include <motors.h>
#include <pico/stdlib.h>
#include <hardware/pwm.h>

void motors_init(uint8_t clock_divider) {
  // Initialize and set direction of GPIO pins.
  gpio_init(RIGHT_MOTOR_DIRECTION);
  gpio_set_dir(RIGHT_MOTOR_DIRECTION, GPIO_OUT);
  gpio_init(LEFT_MOTOR_DIRECTION);
  gpio_set_dir(LEFT_MOTOR_DIRECTION, GPIO_OUT);

  // Set motor PWM GPIO pins
  gpio_set_function(RIGHT_MOTOR_SPEED, GPIO_FUNC_PWM);
  gpio_set_function(LEFT_MOTOR_SPEED, GPIO_FUNC_PWM);

  // Configure PWM slice and get the slice number that controls the speed.
  // The right and left motor speed PWM controllers are in the same "slice",
  // so we need to specify only the right.
  uint32_t slice = pwm_gpio_to_slice_num(RIGHT_MOTOR_SPEED);

  // At 125MHz, with a 12-bit PWM counter, if we divide the clock by 1
  // (no dividing), then the frequency of PWM control is about 30.5 kHz,
  // or 125000000 / 4095.
  pwm_config cfg = pwm_get_default_config();
  pwm_config_set_clkdiv_int_frac(&cfg, clock_divider, 0);
  pwm_config_set_wrap(&cfg, MAX_MOTOR_POWER);
  pwm_init(slice, &cfg, true);
}

void motors_set_power(uint16_t power, bool forward, bool left) {
  if (power > MAX_MOTOR_POWER) power = MAX_MOTOR_POWER;
  if (left) {
    // Note that setting the pin low makes the robot go forward.
    gpio_put(LEFT_MOTOR_DIRECTION, !forward);
    pwm_set_gpio_level(LEFT_MOTOR_SPEED, power);
  } else {
    gpio_put(RIGHT_MOTOR_DIRECTION, !forward);
    pwm_set_gpio_level(RIGHT_MOTOR_SPEED, power);
  }
}