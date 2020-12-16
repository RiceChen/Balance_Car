#ifndef DEVICES_DEV_KEY_H_
#define DEVICES_DEV_KEY_H_

typedef enum
{
    KEY_NULL,
    KEY_ENTRY_DOWN,
    KEY_SWITCH_DOWN,
    KEY_ADD_DOWN,
    KEY_SUB_DOWN,
    KEY_FUNC_OUT,
}KEY_NUMBER;

#define ENTRY           GET_PIN(A, 2)
#define SWITCH          GET_PIN(C, 15)
#define ADD             GET_PIN(C, 14)
#define SUB             GET_PIN(C, 13)

#define ENTRY_VAL       rt_pin_read(ENTRY)
#define SWITCH_VAL      rt_pin_read(SWITCH)
#define ADD_VAL         rt_pin_read(ADD)
#define SUB_VAL         rt_pin_read(SUB)

rt_uint16_t rt_read_key(void);

#endif /* DEVICES_DEV_KEY_H_ */
