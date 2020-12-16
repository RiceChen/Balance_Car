#include <drv_common.h>
#include "sds.h"
#include "dev_ble.h"

static rt_uint16_t crc_check(rt_uint8_t *buff, rt_uint8_t crc_count)
{
    rt_uint16_t crc_temp;
    rt_uint8_t i,j;
    crc_temp = 0xffff;

    for (i=0;i<crc_count; i++)
    {
        crc_temp ^= buff[i];
        for (j=0;j<8;j++)
        {
            if (crc_temp & 0x01)
            {
                crc_temp = (crc_temp >>1 ) ^ 0xa001;
            }
            else
            {
                crc_temp = crc_temp >> 1;
            }
        }
    }
    return crc_temp;
}

void sds_output_data(struct sds_msg *msg)
{
    rt_int32_t temp[4] = {0};
    rt_uint32_t temp1[4] = {0};
    rt_uint8_t data_buf[10] = {0};
    rt_uint8_t i;
    rt_uint16_t crc16 = 0;
    float out_data[4];

    out_data[0] = msg->ch1_data;
    out_data[1] = msg->ch2_data;
    out_data[2] = msg->ch3_data;
    out_data[3] = msg->ch4_data;

    for(i = 0; i < 4; i++)
    {
        temp[i]  = (int)out_data[i];
        temp1[i] = (rt_uint32_t)temp[i];
    }

    for(i = 0; i < 4; i++)
    {
        data_buf[i*2]   = (rt_uint8_t)(temp1[i] % 256);
        data_buf[i*2+1] = (rt_uint8_t)(temp1[i] / 256);
    }

    crc16 = crc_check(data_buf, 8);
    data_buf[8] = crc16 % 256;
    data_buf[9] = crc16 / 256;

    for(i = 0; i < 10; i++)
    {
        rt_ble_uart_send(&data_buf[i], 1);
    }
}
