add_library(robot
#  src/battery.c
#  src/button.c
  src/display.c
  src/font_8x8.c
  src/font_8x16.c
  src/imu.c
  src/ir_sensors.c
  src/motors.c
#  src/rgb_leds.c
  src/sh1106.c
)
pico_generate_pio_header(robot
  ${CMAKE_CURRENT_LIST_DIR}/qtr_sensor_counter.pio
)

pico_generate_pio_header(robot
  ${CMAKE_CURRENT_LIST_DIR}/quadrature_encoder.pio
)

target_include_directories(robot PUBLIC src)
target_link_libraries(robot
  pico_stdlib
  hardware_adc
  hardware_pio
  hardware_pwm
  hardware_spi
  hardware_i2c
)
