#ifndef DRIVERS_DRV_SOFT_I2C_H_
#define DRIVERS_DRV_SOFT_I2C_H_

#define RT_USING_SOFT_I2C1
#define RT_SOFT_I2C1_SCL_PIN            GET_PIN(A, 5)
#define RT_SOFT_I2C1_SDA_PIN            GET_PIN(A, 6)

#define RT_USING_SOFT_I2C2
#define RT_SOFT_I2C2_SCL_PIN            GET_PIN(B, 8)
#define RT_SOFT_I2C2_SDA_PIN            GET_PIN(B, 9)

#define RT_SOFT_I2C_WR                  0x0000
#define RT_SOFT_I2C_RD                  (1u << 0)
#define RT_SOFT_I2C_ADDR_10BIT          (1u << 2)  /* this is a ten bit chip address */
#define RT_SOFT_I2C_NO_START            (1u << 4)
#define RT_SOFT_I2C_IGNORE_NACK         (1u << 5)
#define RT_SOFT_I2C_NO_READ_ACK         (1u << 6)  /* when I2C reading, we do not ACK */

struct rt_soft_i2c_config
{
    char *name;
    rt_uint8_t scl;
    rt_uint8_t sda;
};

struct rt_soft_i2c_msg
{
    rt_uint16_t addr;
    rt_uint16_t flags;
    rt_uint16_t len;
    rt_uint8_t  *buf;
};

struct rt_soft_i2c_drv
{
    struct rt_soft_i2c_config *config;
    rt_uint32_t  retries;
};

#ifdef RT_USING_SOFT_I2C1
#define SOFT_I2C1_CONFIG                                                 \
    {                                                               \
        .name = "soft_i2c1",                                        \
        .scl = RT_SOFT_I2C1_SCL_PIN,                                \
        .sda = RT_SOFT_I2C1_SDA_PIN,                                \
    }
#endif

#ifdef RT_USING_SOFT_I2C2
#define SOFT_I2C2_CONFIG                                                 \
    {                                                               \
        .name = "soft_i2c2",                                        \
        .scl = RT_SOFT_I2C2_SCL_PIN,                                \
        .sda = RT_SOFT_I2C2_SDA_PIN,                                \
    }
#endif


struct rt_soft_i2c_drv *rt_find_soft_i2c(char *name);
rt_size_t rt_soft_i2c_master_xfer(struct rt_soft_i2c_drv *obj,
                                         struct rt_soft_i2c_msg msgs[],
                                         rt_uint32_t            num);

#endif /* DRIVERS_DRV_SOFT_I2C_H_ */
