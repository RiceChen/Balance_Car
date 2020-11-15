#ifndef BC_UPPER_H
#define BC_UPPER_H

#include <QMainWindow>
#include <QTimer>
#include <QChart>
#include <QLineSeries>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "chartview.h"
#include "callout.h"

QT_BEGIN_NAMESPACE
namespace Ui { class bc_upper; }
QT_END_NAMESPACE

#define BC_HEAD     0xbc
#define BC_TAIL     '\n'

typedef enum {
    BC_PARAM_TYPE = 0x01,
    BC_DIRECTION_TYPE,
    BC_WAVEFORM_TYPE,
}BC_DATA_TYPE;

typedef enum {
    BC_BALANCE_P_PARAM_CHANNEL = 0x01,
    BC_BALANCE_I_PARAM_CHANNEL,
    BC_BALANCE_D_PARAM_CHANNEL,
    BC_SPEED_P_PARAM_CHANNEL,
    BC_SPEED_I_PARAM_CHANNEL,
    BC_SPEED_D_PARAM_CHANNEL,
    BC_SPEED_PARAM_CHANNEL,
}BC_PARAM_CHANNEL;

typedef enum {
    BC_UP_DIRECTION_CHANNEL = 0x01,
    BC_DOWN_DIRECTION_CHANNEL,
    BC_LEFT_DIRECTION_CHANNEL,
    BC_RIGHT_DIRECTION_CHANNEL,
    BC_LEFT_UP_DIRECTION_CHANNEL,
    BC_LEFT_DOWN_DIRECTION_CHANNEL,
    BC_RIGHT_UP_DIRECTION_CHANNEL,
    BC_RIGHT_DOWN_DIRECTION_CHANNEL,
}BC_DIRECTION_CHANNEL;

typedef enum {
    BC_BALANCE_PID_WAVEFORM_CHANNEL = 0x01,
    BC_SPEED_PID_WAVEFORM_CHANNEL,
    BC_SPEED_WAVEFORM_CHANNEL,
}BC_WAVEFORM_CHANNEL;

struct data_packet {
  unsigned char head;
  unsigned char type;
  unsigned char channel;
  unsigned char reserver1;
  int data;
  int reserver2 :24;
  int tail :8;
};


class bc_upper : public QMainWindow
{
    Q_OBJECT

public:
    bc_upper(QWidget *parent = nullptr);
    ~bc_upper();

private:
    Ui::bc_upper *ui;

private:
    void chatInit();

    void balanc_pid_updateData(float newData);
    void speed_pid_updateData(float data);
    void speed_updateData(float data);

    void bc_param_handler(struct data_packet *recv_data);
    void bc_direction_handler(struct data_packet *recv_data);
    void bc_waveform_handler(struct data_packet *recv_data);


    QSerialPort bc_serial;

    ChartView *balanc_pid_chartView;
    QChart *balanc_pid_chart;
    Callout *balanc_pid_tip;
    QLineSeries *balanc_pid_series;

    ChartView *speed_pid_chartView;
    QChart *speed_pid_chart;
    Callout *speed_pid_tip;
    QLineSeries *speed_pid_series;

    ChartView *speed_chartView;
    QChart *speed_chart;
    Callout *speed_tip;
    QLineSeries *speed_series;

    struct data_packet send_data;
    struct data_packet recv_data;

private slots:
    void balanc_pid_tipSlot(QPointF position, bool isHovering);
    void speed_pid_tipSlot(QPointF position, bool isHovering);
    void speed_tipSlot(QPointF position, bool isHovering);
    void bc_serial_recv_handler();

    void on_push_scan_clicked();
    void on_push_open_clicked();
    void on_push_balance_p_clicked();
    void on_push_balance_i_clicked();
    void on_push_balance_d_clicked();
    void on_push_speed_p_clicked();
    void on_push_speed_i_clicked();
    void on_push_speed_d_clicked();
    void on_push_speed_clicked();
    void on_push_up_clicked();
    void on_push_down_clicked();
    void on_push_left_clicked();
    void on_push_right_clicked();
    void on_push_left_up_clicked();
    void on_push_right_up_clicked();
    void on_push_left_down_clicked();
    void on_push_right_down_clicked();
};
#endif // BC_UPPER_H
