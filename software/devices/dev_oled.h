#ifndef DEVICES_DEV_OLED_H_
#define DEVICES_DEV_OLED_H_

#define OLED_FILL_WHITE         0xff
#define OLED_FILL_BLACK         0x00

void rt_oled_fill(rt_uint8_t fill_data);
void rt_oled_show_str(rt_uint8_t x, rt_uint8_t y, char *str);
void rt_oled_show_digit(rt_uint8_t x, rt_uint8_t y, int digit);

#endif /* DEVICES_DEV_OLED_H_ */
