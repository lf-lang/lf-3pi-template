/// @file imu.c
/// contains implementations for the imu library
#include <imu.h>
#include <hardware/i2c.h>
#include <hardware/gpio.h>


// Output Data Rate
const uint8_t odr_acc = 0b0011; // 52Hz
const uint8_t odr_gyro = 0b0101; // 208Hz

// Measurement Ranges
const uint8_t fs_acc = 0b00; // +- 2g
const uint8_t fs_gyro = 0b110; // +- 2000 dps

// Sensitivity Multipliers 
const float sen_acc = 0.061; // mg/LSB 
const float sen_gyro = 70; // mdps/LSB

uint imu_init(imu_inst_t* imu_inst) {
    uint8_t w_cmd;
    uint8_t r_buf;

    if (imu_inst->status) {
        return 0;
    }
    
    // i2c peripheral instance 0
    // initialize instance and record baudrate
    imu_inst->set_baud = i2c_init(i2c0, _IMU_I2C0_BAUDRATE);
    
    // sda: data line
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_pull_up(4);
   
    // scl: clk line 
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(5);

    // read from WHO AM I register
    w_cmd = _IMU_REG_WHO_AM_I;
    // keep bus control
    i2c_write_blocking(i2c0, _IMU_ADDR, &w_cmd, 1, true);
    i2c_read_blocking(i2c0, _IMU_ADDR, &r_buf, 1, false);
    imu_inst->status = (r_buf == 0x6C);
    
    if (!imu_inst->status) {
        return -1;
    }

    _imu_reset();
    _imu_set();
    return 0;
}


void _imu_reset() {
    uint8_t cmd_buf[2] = {_IMU_REG_CTRL3_C, 0x00};
    // read and store CTRL3
    i2c_write_blocking(i2c0, _IMU_ADDR, &(cmd_buf[0]), 1, true);
    i2c_read_blocking(i2c0, _IMU_ADDR, &(cmd_buf[1]), 1, false);
    // mask and write
    // write to _CTRL3_C.BOOT (0x80)
    cmd_buf[1] |= 0x80;
    i2c_write_blocking(i2c0, _IMU_ADDR, cmd_buf, 2, false);
    // blocking read until _CTRL3_C.BOOT flag resets
    do {
        i2c_write_blocking(i2c0, _IMU_ADDR, &(cmd_buf[0]), 1, true);
        i2c_read_blocking(i2c0, _IMU_ADDR, &(cmd_buf[1]), 1, false);
    } while(cmd_buf[1] & 0x80);

    /// TODO: sleep and check at end or poll check
    /// use an interrupt to fire when reset complete
    /// callback update the imu_ins->status

    // mask and write
    // write to _CTRL3_C.SW_RESET (0x01)
    cmd_buf[1] |= 0x01;
    i2c_write_blocking(i2c0, _IMU_ADDR, cmd_buf, 2, false);
    // blocking read until _CTRL3_C.SW_RESET flag resets
    do {
        i2c_write_blocking(i2c0, _IMU_ADDR, &(cmd_buf[0]), 1, true);
        i2c_read_blocking(i2c0, _IMU_ADDR, &(cmd_buf[1]), 1, false);
    } while(cmd_buf[1] & 0x01);
}
void _imu_set() {
    // imu general configuration
    uint8_t cmd_buf[2] = {_IMU_REG_CTRL3_C, 0x00};
    // read and store CTRL3
    i2c_write_blocking(i2c0, _IMU_ADDR, &(cmd_buf[0]), 1, true);
    i2c_read_blocking(i2c0, _IMU_ADDR, &(cmd_buf[1]), 1, false);
    // write to _CTRL3_C.BDU (0x40)
    cmd_buf[1] |= 0x40;
    i2c_write_blocking(i2c0, _IMU_ADDR, cmd_buf, 2, false);
    
    uint8_t cmd_acc_buf[2] = {_IMU_REG_CTRL1_XL, 0x00};
    uint8_t cmd_gyro_buf[2] = {_IMU_REG_CTRL2_G, 0x00};
    
    // write to CTRL1_XL
    cmd_acc_buf[1] = (odr_acc << 4 | fs_acc << 2) | 0b00;
    i2c_write_blocking(i2c0, _IMU_ADDR, cmd_acc_buf, 2, false);
    
    // write to CTRL2_G
    cmd_gyro_buf[1] = (odr_gyro << 4 | fs_gyro << 1) | 0b0;
    i2c_write_blocking(i2c0, _IMU_ADDR, cmd_gyro_buf, 2, false);
}

uint imu_read_acc(imu_inst_t* imu_inst, axes_data_t* acc_data) {
    if (!imu_inst->status) {
        return -1;
    }  
    uint8_t reg = _IMU_OUTX_L_XL;
    uint8_t buf[6];
    int16_t raw[3];
    // read from the register 
    i2c_write_blocking(i2c0, _IMU_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c0, _IMU_ADDR, buf, 6, false);
    /// TODO: might be other way
    for (int i = 0; i < 3; i++) {
        raw[i] = (buf[(i * 2) + 1] << 8 | buf[i * 2]);
    }
    acc_data->x = (raw[0] * sen_acc) / 1000.0;
    acc_data->y = (raw[1] * sen_acc) / 1000.0;
    acc_data->z = (raw[2] * sen_acc) / 1000.0;
    return 0;
}

uint imu_read_gyro(imu_inst_t* imu_inst, axes_data_t* gyro_data) {
    if (!imu_inst->status) {
        return -1;
    }  
    uint8_t reg = _IMU_OUTX_L_G;
    uint8_t buf[6];
    int16_t raw[3];
    /// TODO: abstract this subroutine
    /// TODO: add imu_inst checks
    // read from the register 
    i2c_write_blocking(i2c0, _IMU_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c0, _IMU_ADDR, buf, 6, false);
    /// TODO: might be other way
    for (int i = 0; i < 3; i++) {
        raw[i] = (buf[(i * 2) + 1] << 8 | buf[i * 2]);
    }
    gyro_data->x = (raw[0] * sen_gyro) / 1000.0;
    gyro_data->y = (raw[1] * sen_gyro) / 1000.0;
    gyro_data->z = (raw[2] * sen_gyro) / 1000.0;
    return 0;
}