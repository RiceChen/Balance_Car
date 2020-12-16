#include "drv_common.h"
#include "drv_soft_i2c.h"
#include "drv_gpio.h"

enum
{
#ifdef RT_USING_SOFT_I2C1
    I2C1_INDEX,
#endif
#ifdef RT_USING_SOFT_I2C2
    I2C2_INDEX,
#endif
};

static struct rt_soft_i2c_config i2c_config[] =
{
#ifdef RT_USING_SOFT_I2C1
     SOFT_I2C1_CONFIG,
#endif
#ifdef RT_USING_SOFT_I2C2
     SOFT_I2C2_CONFIG
#endif
};

static struct rt_soft_i2c_drv soft_i2c_obj[sizeof(i2c_config) / sizeof(i2c_config[0])] = {0};

static void rt_soft_i2c_configure(struct rt_soft_i2c_drv *obj)
{
    rt_uint8_t scl_pin = obj->config->scl;
    rt_uint8_t sda_pin = obj->config->sda;

    rt_pin_mode(scl_pin, PIN_MODE_OUTPUT_OD);
    rt_pin_mode(sda_pin, PIN_MODE_OUTPUT_OD);

    rt_pin_write(scl_pin, PIN_HIGH);
    rt_pin_write(sda_pin, PIN_HIGH);
}

static void rt_soft_i2c_set_sda(struct rt_soft_i2c_drv *obj, rt_uint32_t state)
{
    rt_uint8_t sda_pin = obj->config->sda;

    if (state)
    {
        rt_pin_write(sda_pin, PIN_HIGH);
    }
    else
    {
        rt_pin_write(sda_pin, PIN_LOW);
    }
}

static void rt_soft_i2c_set_scl(struct rt_soft_i2c_drv *obj, rt_uint32_t state)
{
    rt_uint8_t scl_pin = obj->config->scl;

    if (state)
    {
        rt_pin_write(scl_pin, PIN_HIGH);
    }
    else
    {
        rt_pin_write(scl_pin, PIN_LOW);
    }
}

static rt_uint32_t rt_soft_i2c_get_sda(struct rt_soft_i2c_drv *obj)
{
    rt_uint8_t sda_pin = obj->config->sda;

    return rt_pin_read(sda_pin);
}

static rt_uint32_t rt_soft_i2c_get_scl(struct rt_soft_i2c_drv *obj)
{
    rt_uint8_t scl_pin = obj->config->scl;

    return rt_pin_read(scl_pin);
}

static void rt_soft_i2c_udelay(rt_uint32_t us)
{
    rt_hw_us_delay(us);
}

#define SET_SDA(obj, val)   rt_soft_i2c_set_sda(obj, val)
#define SET_SCL(obj, val)   rt_soft_i2c_set_scl(obj, val)
#define GET_SDA(obj)        rt_soft_i2c_get_sda(obj)
#define GET_SCL(obj)        rt_soft_i2c_get_scl(obj)

#define SDA_L(obj)          SET_SDA(obj, 0)
#define SDA_H(obj)          SET_SDA(obj, 1)
#define SCL_L(obj)          SET_SCL(obj, 0)

static rt_err_t SCL_H(struct rt_soft_i2c_drv *obj)
{
    rt_tick_t start;

    SET_SCL(obj, 1);

    if(rt_soft_i2c_get_scl(obj))
    {
        goto done;
    }

    start = rt_tick_get();
    while (!GET_SCL(obj))
    {
        if ((rt_tick_get() - start) > 100)
            return -RT_ETIMEOUT;
        rt_thread_delay(100);
    }
done:
    rt_soft_i2c_udelay(1);

    return RT_EOK;
}

static void rt_soft_i2c_start(struct rt_soft_i2c_drv *obj)
{
    SDA_L(obj);
    rt_soft_i2c_udelay(1);
    SCL_L(obj);
}

static void rt_soft_i2c_restart(struct rt_soft_i2c_drv *obj)
{
    SDA_H(obj);
    SCL_H(obj);
    rt_soft_i2c_udelay(1);

    SDA_L(obj);
    rt_soft_i2c_udelay(1);
    SCL_L(obj);
}

static void rt_soft_i2c_stop(struct rt_soft_i2c_drv *obj)
{
    SDA_L(obj);
    rt_soft_i2c_udelay(1);
    SCL_H(obj);
    rt_soft_i2c_udelay(1);
    SDA_H(obj);
    rt_soft_i2c_udelay(1);
}

rt_inline rt_bool_t rt_soft_i2c_waitack(struct rt_soft_i2c_drv *obj)
{
    rt_bool_t ack;

    SDA_H(obj);
    rt_soft_i2c_udelay(1);

    if (SCL_H(obj) < 0)
    {
        return -RT_ETIMEOUT;
    }
    ack = !GET_SDA(obj);
    SCL_L(obj);

    return ack;
}

static rt_int32_t rt_soft_i2c_writeb(struct rt_soft_i2c_drv *obj, rt_uint8_t data)
{
    rt_int32_t i;
    rt_uint8_t bit;

    for (i = 7; i >= 0; i--)
    {
        SCL_L(obj);
        bit = (data >> i) & 1;
        SET_SDA(obj, bit);
        rt_soft_i2c_udelay(1);
        if (SCL_H(obj) < 0)
        {
            return -RT_ETIMEOUT;
        }
    }
    SCL_L(obj);
    rt_soft_i2c_udelay(1);

    return rt_soft_i2c_waitack(obj);
}

static rt_int32_t rt_soft_i2c_readb(struct rt_soft_i2c_drv *obj)
{
    rt_uint8_t i;
    rt_uint8_t data = 0;

    SDA_H(obj);
    rt_soft_i2c_udelay(1);
    for (i = 0; i < 8; i++)
    {
        data <<= 1;
        if (SCL_H(obj) < 0)
        {
            return -RT_ETIMEOUT;
        }
        if (GET_SDA(obj))
            data |= 1;
        SCL_L(obj);
        rt_soft_i2c_udelay(1);
    }

    return data;
}

static rt_size_t rt_soft_i2c_send_bytes(struct rt_soft_i2c_drv *obj,
                                        struct rt_soft_i2c_msg *msg)
{
    rt_int32_t ret;
    rt_size_t bytes = 0;
    const rt_uint8_t *ptr = msg->buf;
    rt_int32_t count = msg->len;
    rt_uint16_t ignore_nack = msg->flags & RT_SOFT_I2C_IGNORE_NACK;

    while (count > 0)
    {
        ret = rt_soft_i2c_writeb(obj, *ptr);

        if ((ret > 0) || (ignore_nack && (ret == 0)))
        {
            count --;
            ptr ++;
            bytes ++;
        }
        else if (ret == 0)
        {
            return 0;
        }
        else
        {
            return ret;
        }
    }

    return bytes;
}

static rt_err_t rt_soft_i2c_send_ack_or_nack(struct rt_soft_i2c_drv *obj, int ack)
{
    if (ack)
        SET_SDA(obj, 0);
    rt_soft_i2c_udelay(1);
    if (SCL_H(obj) < 0)
    {
        return -RT_ETIMEOUT;
    }
    SCL_L(obj);

    return RT_EOK;
}

static rt_size_t rt_soft_i2c_recv_bytes(struct rt_soft_i2c_drv *obj,
                                        struct rt_soft_i2c_msg *msg)
{
    rt_int32_t val;
    rt_int32_t bytes = 0;   /* actual bytes */
    rt_uint8_t *ptr = msg->buf;
    rt_int32_t count = msg->len;
    const rt_uint32_t flags = msg->flags;

    while (count > 0)
    {
        val = rt_soft_i2c_readb(obj);
        if (val >= 0)
        {
            *ptr = val;
            bytes ++;
        }
        else
        {
            break;
        }

        ptr ++;
        count --;
        if (!(flags & RT_SOFT_I2C_NO_READ_ACK))
        {
            val = rt_soft_i2c_send_ack_or_nack(obj, count);
            if (val < 0)
                return val;
        }
    }

    return bytes;
}

static rt_int32_t rt_soft_i2c_send_address(struct rt_soft_i2c_drv *obj,
                                           rt_uint8_t addr, rt_int32_t retries)
{
    rt_int32_t i;
    rt_err_t ret = 0;

    for (i = 0; i <= retries; i++)
    {
        ret = rt_soft_i2c_writeb(obj, addr);
        if (ret == 1 || i == retries)
            break;
        rt_soft_i2c_stop(obj);
        rt_soft_i2c_udelay(1);
        rt_soft_i2c_start(obj);
    }

    return ret;
}

static rt_err_t rt_soft_i2c_bit_send_address(struct rt_soft_i2c_drv *obj,
                                             struct rt_soft_i2c_msg *msg)
{
    rt_uint16_t flags = msg->flags;
    rt_uint16_t ignore_nack = msg->flags & RT_SOFT_I2C_IGNORE_NACK;

    rt_uint8_t addr1, addr2;
    rt_int32_t retries;
    rt_err_t ret;

    retries = ignore_nack ? 0 : obj->retries;

    if (flags & RT_SOFT_I2C_ADDR_10BIT)
    {
        addr1 = 0xf0 | ((msg->addr >> 7) & 0x06);
        addr2 = msg->addr & 0xff;

        ret = rt_soft_i2c_send_address(obj, addr1, retries);
        if ((ret != 1) && !ignore_nack)
        {
            return -RT_EIO;
        }

        ret = rt_soft_i2c_writeb(obj, addr2);
        if ((ret != 1) && !ignore_nack)
        {
            return -RT_EIO;
        }
        if (flags & RT_SOFT_I2C_RD)
        {
            rt_soft_i2c_restart(obj);
            addr1 |= 0x01;
            ret = rt_soft_i2c_send_address(obj, addr1, retries);
            if ((ret != 1) && !ignore_nack)
            {
                return -RT_EIO;
            }
        }
    }
    else
    {
        addr1 = msg->addr << 1;
        if (flags & RT_SOFT_I2C_RD)
            addr1 |= 1;
        ret = rt_soft_i2c_send_address(obj, addr1, retries);
        if ((ret != 1) && !ignore_nack)
            return -RT_EIO;
    }

    return RT_EOK;
}

rt_size_t rt_soft_i2c_master_xfer(struct rt_soft_i2c_drv *obj,
                                  struct rt_soft_i2c_msg msgs[],
                                  rt_uint32_t            num)
{
    struct rt_soft_i2c_msg *msg;
    rt_int32_t i, ret;
    rt_uint16_t ignore_nack;

    rt_soft_i2c_start(obj);
    for (i = 0; i < num; i++)
    {
        msg = &msgs[i];
        ignore_nack = msg->flags & RT_SOFT_I2C_IGNORE_NACK;
        if (!(msg->flags & RT_SOFT_I2C_NO_START))
        {
            if (i)
            {
                rt_soft_i2c_restart(obj);
            }
            ret = rt_soft_i2c_bit_send_address(obj, msg);
            if ((ret != RT_EOK) && !ignore_nack)
            {
                goto out;
            }
        }
        if (msg->flags & RT_SOFT_I2C_RD)
        {
            ret = rt_soft_i2c_recv_bytes(obj, msg);
            if (ret >= 1)
                ;
            if (ret < msg->len)
            {
                if (ret >= 0)
                    ret = -RT_EIO;
                goto out;
            }
        }
        else
        {
            ret = rt_soft_i2c_send_bytes(obj, msg);
            if (ret >= 1)
                ;
            if (ret < msg->len)
            {
                if (ret >= 0)
                    ret = -RT_ERROR;
                goto out;
            }
        }
    }
    ret = i;

out:
    rt_soft_i2c_stop(obj);

    return ret;
}

struct rt_soft_i2c_drv *rt_find_soft_i2c(char *name)
{
    rt_size_t obj_num = sizeof(soft_i2c_obj) / sizeof(struct rt_soft_i2c_drv);

    for(int i = 0; i < obj_num; i++)
    {
        if(rt_strncmp(soft_i2c_obj[i].config->name, name, RT_NAME_MAX) == 0)
        {
            return &soft_i2c_obj[i];
        }
    }
    return RT_NULL;
}

int rt_soft_i2c_init(void)
{
    rt_size_t obj_num = sizeof(soft_i2c_obj) / sizeof(struct rt_soft_i2c_drv);

    for(int i = 0; i < obj_num; i++)
    {
        soft_i2c_obj[i].config = &i2c_config[i];
        rt_soft_i2c_configure(&soft_i2c_obj[i]);
    }

    return RT_EOK;
}
INIT_PREV_EXPORT(rt_soft_i2c_init);

