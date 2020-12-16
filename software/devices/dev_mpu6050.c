#include <drv_common.h>
#include "dev_mpu6050.h"
#include "drv_soft_i2c.h"

struct rt_mpu6050_dev mpu6050_dev;

static rt_err_t rt_mpu6050_write_reg(struct rt_mpu6050_dev *dev, rt_uint8_t reg, rt_uint8_t data)
{
    rt_uint8_t buf[2] = {reg, data};
    struct rt_soft_i2c_msg msgs;

    msgs.addr = MPU6050_ADDR;
    msgs.flags = RT_SOFT_I2C_WR;
    msgs.buf = buf;
    msgs.len = 2;

    return (rt_soft_i2c_master_xfer(dev->i2c_dev, &msgs, 1) == 1) ? RT_EOK : RT_ERROR;
}

static rt_err_t rt_mpu6050_read_reg(struct rt_mpu6050_dev *dev, rt_uint8_t reg, rt_uint8_t len, rt_uint8_t *data)
{
    struct rt_soft_i2c_msg msgs[2];

    msgs[0].addr = MPU6050_ADDR;
    msgs[0].flags = RT_SOFT_I2C_WR;
    msgs[0].buf = &reg;
    msgs[0].len = 1;

    msgs[1].addr = MPU6050_ADDR;
    msgs[1].flags = RT_SOFT_I2C_RD;
    msgs[1].buf = data;
    msgs[1].len = len;

    return (rt_soft_i2c_master_xfer(dev->i2c_dev, msgs, 2) == 2) ? RT_EOK : RT_ERROR;
}

static rt_uint8_t rt_mpu6050_write_bit(struct rt_mpu6050_dev *dev, rt_uint8_t reg,
                                       rt_uint8_t bit, rt_uint8_t data)
{
    rt_uint8_t byte;
    rt_err_t res;

    res = rt_mpu6050_read_reg(dev, reg, 1, &byte);
    if(res !=RT_EOK)
    {
        return res;
    }
    byte = (data != 0) ? (byte | (1 << bit)) : (byte & ~(1 << bit));
    return rt_mpu6050_write_reg(dev, reg, byte);
}

static rt_uint8_t rt_mpu6050_read_bit(struct rt_mpu6050_dev *dev, rt_uint8_t reg,
                                rt_uint8_t bit,rt_uint8_t *data)
{
    rt_uint8_t byte;
    rt_err_t res;

    res = rt_mpu6050_read_reg(dev, reg, 1, &byte);
    if(res != RT_EOK)
    {
        return res;
    }

    *data = byte & (1 << bit);

    return RT_EOK;
}

static rt_err_t rt_mpu6050_write_bits(struct rt_mpu6050_dev *dev, rt_uint8_t reg, rt_uint8_t start_bit, rt_uint8_t len, rt_uint8_t data)
{
    rt_uint8_t byte, mask;
    rt_err_t res;

    res = rt_mpu6050_read_reg(dev, reg, 1, &byte);
    if (res != RT_EOK)
    {
        return res;
    }

    mask = ((1 << len) - 1) << (start_bit - len + 1);
    data <<= (start_bit - len + 1);
    data &= mask;
    byte &= ~(mask);
    byte |= data;

    return rt_mpu6050_write_reg(dev, reg, byte);
}

static rt_err_t rt_mpu6050_read_bits(struct rt_mpu6050_dev *dev, rt_uint8_t reg, rt_uint8_t start_bit, rt_uint8_t len, rt_uint8_t *data)
{
    rt_uint8_t byte, mask;
    rt_err_t res;

    res = rt_mpu6050_read_reg(dev, reg, 1, &byte);
    if (res != RT_EOK)
    {
        return res;
    }

    mask = ((1 << len) - 1) << (start_bit - len + 1);
    byte &= mask;
    byte >>= (start_bit - len + 1);
    *data = byte;

    return RT_EOK;
}

rt_err_t rt_mpu6050_get_accel_raw(struct mpu6050_3axes *accel)
{
    rt_uint8_t buffer[6];
    rt_err_t res;

    res = rt_mpu6050_read_reg(&mpu6050_dev, MPU6050_RA_ACCEL_XOUT_H, 6, buffer);
    if (res != RT_EOK)
    {
        return res;
    }

    accel->x = ((rt_uint16_t)buffer[0] << 8) + buffer[1];
    accel->y = ((rt_uint16_t)buffer[2] << 8) + buffer[3];
    accel->z = ((rt_uint16_t)buffer[4] << 8) + buffer[5];

    return RT_EOK;
}

rt_err_t rt_mpu6050_get_gyro_raw(struct mpu6050_3axes *gyro)
{
    rt_uint8_t buffer[6];
    rt_err_t res;

    res = rt_mpu6050_read_reg(&mpu6050_dev, MPU6050_RA_GYRO_XOUT_H, 6, buffer);
    if (res != RT_EOK)
    {
        return res;
    }

    gyro->x = ((rt_uint16_t)buffer[0] << 8) + buffer[1];
    gyro->y = ((rt_uint16_t)buffer[2] << 8) + buffer[3];
    gyro->z = ((rt_uint16_t)buffer[4] << 8) + buffer[5];

    return RT_EOK;
}

rt_err_t rt_mpu6050_get_temp_raw(rt_int16_t *temp)
{
    rt_uint8_t buffer[2];
    rt_err_t res;

    res = rt_mpu6050_read_reg(&mpu6050_dev, MPU6050_RA_TEMP_OUT_H, 2, buffer);
    if (res != RT_EOK)
    {
        return res;
    }

    *temp = ((rt_uint16_t)buffer[0] << 8) + buffer[1];

    return RT_EOK;
}

rt_err_t rt_mpu6050_get_param(enum mpu6050_cmd cmd, rt_uint16_t *param)
{
    rt_uint8_t data = 0;
    rt_err_t res = RT_EOK;

    RT_ASSERT(mpu6050_dev.i2c_dev);

    switch (cmd)
    {
    case MPU6050_GYRO_RANGE:  /* Gyroscope full scale range */
        res = rt_mpu6050_read_bits(&mpu6050_dev, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, &data);
        *param = data;
        break;
    case MPU6050_ACCEL_RANGE: /* Accelerometer full scale range */
        res = rt_mpu6050_read_bits(&mpu6050_dev, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, &data);
        *param = data;
        break;
    case MPU6050_DLPF_CONFIG: /* Digital Low Pass Filter */
        res = rt_mpu6050_read_bits(&mpu6050_dev, MPU6050_RA_CONFIG, MPU6050_CFG_DLPF_CFG_BIT, MPU6050_CFG_DLPF_CFG_LENGTH, &data);
        *param = data;
        break;
    case MPU6050_SAMPLE_RATE: /* Sample Rate */
        /* Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV) */
        res = rt_mpu6050_read_bits(&mpu6050_dev, MPU6050_RA_CONFIG, MPU6050_CFG_DLPF_CFG_BIT, MPU6050_CFG_DLPF_CFG_LENGTH, &data);
        if (res != RT_EOK)
        {
            break;
        }

        if (data == 0 || data == 7) /* dlpf is disable */
        {
            res = rt_mpu6050_read_reg(&mpu6050_dev, MPU6050_RA_SMPLRT_DIV, 1, &data);
            *param = 8000 / (data + 1);
        }
        else /* dlpf is enable */
        {
            res = rt_mpu6050_read_reg(&mpu6050_dev, MPU6050_RA_SMPLRT_DIV, 1, &data);
            *param = 1000 / (data + 1);
        }
        break;
    case MPU6050_SLEEP: /* sleep mode */
        res = rt_mpu6050_read_bit(&mpu6050_dev, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, &data);
        *param = data;
        break;
    }

    return res;
}

rt_err_t rt_mpu6050_set_param(enum mpu6050_cmd cmd, rt_uint16_t param)
{
    rt_uint8_t data = 0;
    rt_err_t res = RT_EOK;

    RT_ASSERT(mpu6050_dev.i2c_dev);

    switch (cmd)
    {
    case MPU6050_GYRO_RANGE:  /* Gyroscope full scale range */
        res = rt_mpu6050_write_bits(&mpu6050_dev, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, param);
        mpu6050_dev.config.gyro_range = param;
        break;
    case MPU6050_ACCEL_RANGE: /* Accelerometer full scale range */
        res = rt_mpu6050_write_bits(&mpu6050_dev, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, param);
        mpu6050_dev.config.accel_range = param;
        break;
    case MPU6050_DLPF_CONFIG: /* Digital Low Pass Filter */
        res = rt_mpu6050_write_bits(&mpu6050_dev, MPU6050_RA_CONFIG, MPU6050_CFG_DLPF_CFG_BIT, MPU6050_CFG_DLPF_CFG_LENGTH, param);
        break;
    case MPU6050_SAMPLE_RATE: /* Sample Rate —— 16-bit unsigned value.
                                 Sample Rate = [1000 -  4]HZ when dlpf is enable
                                 Sample Rate = [8000 - 32]HZ when dlpf is disable */

        //Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV)
        res = rt_mpu6050_read_bits(&mpu6050_dev, MPU6050_RA_CONFIG, MPU6050_CFG_DLPF_CFG_BIT, MPU6050_CFG_DLPF_CFG_LENGTH, &data);
        if (res != RT_EOK)
        {
            break;
        }

        if (data == 0 || data == 7) /* dlpf is disable */
        {
            if (param > 8000)
                data = 0;
            else if (param < 32)
                data = 0xFF;
            else
                data = 8000 / param - 1;
        }
        else /* dlpf is enable */
        {
            if (param > 1000)
                data = 0;
            else if (param < 4)
                data = 0xFF;
            else
                data = 1000 / param - 1;
        }
        res = rt_mpu6050_write_reg(&mpu6050_dev, MPU6050_RA_SMPLRT_DIV, data);
        break;
    case MPU6050_SLEEP: /* Configure sleep mode */
        res = rt_mpu6050_write_bit(&mpu6050_dev, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, param);
        break;
    }

    return res;
}

int rt_mpu6050_dev_init(void)
{
    int res = 0;
    rt_uint8_t reg = 0xFF;

    mpu6050_dev.i2c_dev = rt_find_soft_i2c("soft_i2c2");
    if(mpu6050_dev.i2c_dev == RT_NULL)
    {
        rt_kprintf("find soft i2c2 fail\n");
        return RT_ERROR;
    }

    if(rt_mpu6050_read_reg(&mpu6050_dev, MPU6050_RA_WHO_AM_I, 1, &reg) != RT_EOK)
    {
        rt_kprintf("failed to read device id!\n");
        return RT_ERROR;
    }

    if(MPU6050_WHO_AM_I != reg)
    {
        rt_kprintf("find device mpu6050 fail!\n");
        return RT_ERROR;
    }

    res += rt_mpu6050_get_param(MPU6050_ACCEL_RANGE, &mpu6050_dev.config.accel_range);
    res += rt_mpu6050_get_param(MPU6050_GYRO_RANGE, &mpu6050_dev.config.gyro_range);

    res += rt_mpu6050_write_bits(&mpu6050_dev, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT,
                                MPU6050_PWR1_CLKSEL_LENGTH, MPU6050_CLOCK_PLL_XGYRO);
    res += rt_mpu6050_set_param( MPU6050_GYRO_RANGE, MPU6050_GYRO_RANGE_250DPS);
    res += rt_mpu6050_set_param(MPU6050_ACCEL_RANGE, MPU6050_ACCEL_RANGE_2G);
    res += rt_mpu6050_set_param(MPU6050_SLEEP, MPU6050_SLEEP_DISABLE);

    if(res != RT_EOK)
    {
        rt_kprintf("Error in device initialization!\n");
        return RT_ERROR;
    }
    rt_kprintf("mpu6050 init success!\n");

    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_mpu6050_dev_init);
