/// @file imu.h
/// Methods for accessing accelerometer and gyro data from
/// lsm6dso sensor
// https://www.pololu.com/file/0J1899/lsm6dso.pdf

#pragma once
#include <hardware/i2c.h>

// 100KHz I2C Bus0 baudrate 
#define _IMU_I2C0_BAUDRATE 400000
#define _IMU_ADDR 0b1101011

#define _IMU_REG_WHO_AM_I 0x0F
#define _IMU_REG_CTRL1_XL 0x10
#define _IMU_REG_CTRL2_G 0x11
#define _IMU_REG_CTRL3_C 0x12
#define _IMU_STATUS_REG 0x1E
#define _IMU_OUTX_L_G 0x22
#define _IMU_OUTX_L_XL 0x28

typedef struct _imu_inst {
    uint32_t set_baud;
    bool status;
} imu_inst_t;

typedef struct _axes_data {
    float x;
    float y;
    float z;
} axes_data_t;

// TODO: determine return type for error reporting and
// fault handling

/// @brief 
/// initialize the i2c instance and set pins
uint imu_init(imu_inst_t* imu_inst);

/// @brief fills the provided memory space
/// with the acceleration in g/s
/// for each axis.
/// @param acc_data 
uint imu_read_acc(imu_inst_t* imu_inst, axes_data_t* acc_data);

/// @brief fills the provided memory space
/// with the gyro data in deg/s
/// for each axis.
/// @param gyro_data 
uint imu_read_gyro(imu_inst_t* imu_inst, axes_data_t* gyro_data);

// private helpers
/// @brief sets configuration registers
void _imu_set();

/// @brief write to imu reset register on i2c
void _imu_reset();