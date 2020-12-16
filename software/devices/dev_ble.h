#ifndef DEVICES_DEV_BLE_H_
#define DEVICES_DEV_BLE_H_

#define BLE_HEAD     0xbc
#define BLE_TAIL     '\n'

typedef enum {
    BLE_PARAM_TYPE = 0x01,
    BLE_DIRECTION_TYPE,
    BLE_WAVEFORM_TYPE,
}BLE_DATA_TYPE;

typedef enum {
    BLE_BALANCE_P_PARAM_CHANNEL = 0x01,
    BLE_BALANCE_I_PARAM_CHANNEL,
    BLE_BALANCE_D_PARAM_CHANNEL,
    BLE_SPEED_P_PARAM_CHANNEL,
    BLE_SPEED_I_PARAM_CHANNEL,
    BLE_SPEED_D_PARAM_CHANNEL,
    BLE_SPEED_PARAM_CHANNEL,
}BLE_PARAM_CHANNEL;

typedef enum {
    BLE_UP_DIRECTION_CHANNEL = 0x01,
    BLE_DOWN_DIRECTION_CHANNEL,
    BLE_LEFT_DIRECTION_CHANNEL,
    BLE_RIGHT_DIRECTION_CHANNEL,
}BLE_DIRECTION_CHANNEL;

typedef enum {
    BLE_BALANCE_PID_WAVEFORM_CHANNEL = 0x01,
    BLE_SPEED_PID_WAVEFORM_CHANNEL,
    BLE_SPEED_WAVEFORM_CHANNEL,
}BLE_WAVEFORM_CHANNEL;

struct ble_data_packet {
    unsigned char head;
    unsigned char type;
    unsigned char channel;
    unsigned char reserver1;
    int data;
    int reserver2 :24;
    int tail :8;
};

#define BLE_DATA_PACKET_SIZE    sizeof(struct ble_data_packet)

typedef void (*ble_recv_cb_func)(void * recv_buff, rt_size_t size);

void ble_recv_cb_register(ble_recv_cb_func recv_cb);

int rt_ble_uart_send(rt_uint8_t *data, rt_size_t len);

#endif /* DEVICES_DEV_BLE_H_ */
