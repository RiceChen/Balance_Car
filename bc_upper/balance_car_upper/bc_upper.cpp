#include "bc_upper.h"
#include "ui_bc_upper.h"

#include <QMessageBox>
#include <QDebug>

bc_upper::bc_upper(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::bc_upper),
    balanc_pid_chart(new QChart),
    speed_pid_chart(new QChart),
    speed_chart(new QChart)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(), this->height());

    chatInit();
}

void bc_upper::on_push_open_clicked()
{
    if(ui->push_open->text() == QString(tr("打开")))
    {
        bc_serial.setPortName(ui->cb_port->currentText());
        bc_serial.setBaudRate(QSerialPort::Baud115200);
        bc_serial.setDataBits(QSerialPort::Data8);
        bc_serial.setStopBits(QSerialPort::OneStop);
        bc_serial.setParity(QSerialPort::NoParity);
        bc_serial.setFlowControl(QSerialPort::NoFlowControl);
        if(!bc_serial.open(QIODevice::ReadWrite))
        {
            QMessageBox::about(NULL, "提示", "无法打开串口");
            return;
        }
        ui->cb_port->setEnabled(false);
        ui->push_open->setText(QString(tr("关闭")));
        connect(&bc_serial, SIGNAL(readyRead()), this, SLOT(bc_serial_recv_handler()));
    }
    else
    {
        bc_serial.close();
        ui->cb_port->setEnabled(true);
        ui->push_open->setText(QString("打开"));
    }
}

void bc_upper::on_push_scan_clicked()
{
    ui->cb_port->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->cb_port->addItem(info.portName());
    }
}

void bc_upper::bc_serial_recv_handler()
{
    QByteArray recvData = bc_serial.readAll();

    memcpy(&recv_data, recvData, sizeof(struct data_packet));

    if(recv_data.head == BC_HEAD && recv_data.tail == BC_TAIL)
    {
        switch(recv_data.type)
        {
        case BC_PARAM_TYPE:
            bc_param_handler(&recv_data);
            break;
        case BC_WAVEFORM_TYPE:
            bc_waveform_handler(&recv_data);
            break;
        default:

            break;
        }
    }

    recvData.clear();
}

void bc_upper::bc_param_handler(struct data_packet *recv_data)
{
    switch(recv_data->channel)
    {
    case BC_BALANCE_P_PARAM_CHANNEL:
        ui->lb_balance_p->setText("平衡P:" + QString::number(recv_data->data, 10));
        break;
    case BC_BALANCE_I_PARAM_CHANNEL:
        ui->lb_balance_p->setText("平衡I:" + QString::number(recv_data->data, 10));
        break;
    case BC_BALANCE_D_PARAM_CHANNEL:
        ui->lb_balance_p->setText("平衡D:" + QString::number(recv_data->data, 10));
        break;
    case BC_SPEED_P_PARAM_CHANNEL:
        ui->lb_balance_p->setText("速度P:" + QString::number(recv_data->data, 10));
        break;
    case BC_SPEED_I_PARAM_CHANNEL:
        ui->lb_balance_p->setText("速度I:" + QString::number(recv_data->data, 10));
        break;
    case BC_SPEED_D_PARAM_CHANNEL:
        ui->lb_balance_p->setText("速度D:" + QString::number(recv_data->data, 10));
        break;
    case BC_SPEED_PARAM_CHANNEL:
        ui->lb_balance_p->setText("速度:" + QString::number(recv_data->data, 10));
        break;
    default:

        break;
    }
}

void bc_upper::bc_waveform_handler(struct data_packet *recv_data)
{
    switch(recv_data->channel)
    {
    case BC_BALANCE_PID_WAVEFORM_CHANNEL:
        balanc_pid_updateData(recv_data->data);
        break;
    case BC_SPEED_PID_WAVEFORM_CHANNEL:
        speed_pid_updateData(recv_data->data);
        break;
    case BC_SPEED_WAVEFORM_CHANNEL:
        speed_updateData(recv_data->data);
        break;
    default:

        break;
    }
}

// balance pid
void bc_upper::balanc_pid_updateData(float newData)
{
    QVector<QPointF> oldData = balanc_pid_series->pointsVector();
    QVector<QPointF> data;

    if (oldData.size() < 1000) {
        data = balanc_pid_series->pointsVector();
    } else {
        for (int i = 1; i < oldData.size(); ++i) {
            data.append(QPointF(i - 1 , oldData.at(i).y()));
        }
    }

    qint64 size = data.size();

    for(int i = 0; i < 1; ++i){
        data.append(QPointF(i + size, newData));
    }

    balanc_pid_series->replace(data);
}


void bc_upper::balanc_pid_tipSlot(QPointF position, bool isHovering)
{
    if (balanc_pid_tip == 0)
        balanc_pid_tip = new Callout(balanc_pid_chart);

    if (isHovering) {
        balanc_pid_tip->setText(QString("X: %1 \nY: %2 ").arg(position.x()).arg(position.y()));
        balanc_pid_tip->setAnchor(position);
        balanc_pid_tip->setZValue(11);
        balanc_pid_tip->updateGeometry();
        balanc_pid_tip->show();
    } else {
        balanc_pid_tip->hide();
    }
}

// speed pid
void bc_upper::speed_pid_updateData(float newData)
{
    QVector<QPointF> oldData = speed_pid_series->pointsVector();
    QVector<QPointF> data;

    if (oldData.size() < 1000) {
        data = speed_pid_series->pointsVector();
    } else {
        for (int i = 1; i < oldData.size(); ++i) {
            data.append(QPointF(i - 1 , oldData.at(i).y()));
        }
    }

    qint64 size = data.size();

    for(int i = 0; i < 1; ++i){
        data.append(QPointF(i + size, newData));
    }

    speed_pid_series->replace(data);
}


void bc_upper::speed_pid_tipSlot(QPointF position, bool isHovering)
{
    if (speed_pid_tip == 0)
        speed_pid_tip = new Callout(balanc_pid_chart);

    if (isHovering) {
        speed_pid_tip->setText(QString("X: %1 \nY: %2 ").arg(position.x()).arg(position.y()));
        speed_pid_tip->setAnchor(position);
        speed_pid_tip->setZValue(11);
        speed_pid_tip->updateGeometry();
        speed_pid_tip->show();
    } else {
        speed_pid_tip->hide();
    }
}

// speed
void bc_upper::speed_updateData(float newData)
{
    QVector<QPointF> oldData = speed_series->pointsVector();
    QVector<QPointF> data;

    if (oldData.size() < 1000) {
        data = speed_series->pointsVector();
    } else {
        for (int i = 1; i < oldData.size(); ++i) {
            data.append(QPointF(i - 1 , oldData.at(i).y()));
        }
    }

    qint64 size = data.size();

    for(int i = 0; i < 1; ++i){
        data.append(QPointF(i + size, newData));
    }

    speed_series->replace(data);
}


void bc_upper::speed_tipSlot(QPointF position, bool isHovering)
{
    if (speed_tip == 0)
        speed_tip = new Callout(balanc_pid_chart);

    if (isHovering) {
        speed_tip->setText(QString("X: %1 \nY: %2 ").arg(position.x()).arg(position.y()));
        speed_tip->setAnchor(position);
        speed_tip->setZValue(11);
        speed_tip->updateGeometry();
        speed_tip->show();
    } else {
        speed_tip->hide();
    }
}

void bc_upper::chatInit()
{
    balanc_pid_series = new QLineSeries;
    balanc_pid_chart->addSeries(balanc_pid_series);

    balanc_pid_chart->createDefaultAxes();
    balanc_pid_chart->axisY()->setRange(-10, 10);
    balanc_pid_chart->axisX()->setRange(0, 1000);

    balanc_pid_chart->axisX()->setTitleFont(QFont("Microsoft YaHei", 7, QFont::Normal, true));
    balanc_pid_chart->axisY()->setTitleFont(QFont("Microsoft YaHei", 7, QFont::Normal, true));
    balanc_pid_chart->axisX()->setTitleText("平衡PID");
    balanc_pid_chart->axisY()->setTitleText("数值");

    balanc_pid_chart->axisX()->setGridLineVisible(false);
    balanc_pid_chart->axisY()->setGridLineVisible(false);

    /* hide legend */
    balanc_pid_chart->legend()->hide();

    balanc_pid_chartView = new ChartView(balanc_pid_chart);
    balanc_pid_chartView->setRenderHint(QPainter::Antialiasing);

    //---------------------------------------------------------
    speed_pid_series = new QLineSeries;
    speed_pid_chart->addSeries(speed_pid_series);

    speed_pid_chart->createDefaultAxes();
    speed_pid_chart->axisY()->setRange(-10, 10);
    speed_pid_chart->axisX()->setRange(0, 1000);

    speed_pid_chart->axisX()->setTitleFont(QFont("Microsoft YaHei", 7, QFont::Normal, true));
    speed_pid_chart->axisY()->setTitleFont(QFont("Microsoft YaHei", 7, QFont::Normal, true));
    speed_pid_chart->axisX()->setTitleText("速度PID");
    speed_pid_chart->axisY()->setTitleText("数值");

    speed_pid_chart->axisX()->setGridLineVisible(false);
    speed_pid_chart->axisY()->setGridLineVisible(false);

    /* hide legend */
    speed_pid_chart->legend()->hide();

    speed_pid_chartView = new ChartView(speed_pid_chart);
    speed_pid_chartView->setRenderHint(QPainter::Antialiasing);

    //---------------------------------------------------------
    speed_series = new QLineSeries;
    speed_chart->addSeries(speed_series);

    speed_chart->createDefaultAxes();
    speed_chart->axisY()->setRange(-10, 10);
    speed_chart->axisX()->setRange(0, 1000);

    speed_chart->axisX()->setTitleFont(QFont("Microsoft YaHei", 7, QFont::Normal, true));
    speed_chart->axisY()->setTitleFont(QFont("Microsoft YaHei", 7, QFont::Normal, true));
    speed_chart->axisX()->setTitleText("速度");
    speed_chart->axisY()->setTitleText("数值");

    speed_chart->axisX()->setGridLineVisible(false);
    speed_chart->axisY()->setGridLineVisible(false);

    /* hide legend */
    speed_chart->legend()->hide();

    speed_chartView = new ChartView(speed_chart);
    speed_chartView->setRenderHint(QPainter::Antialiasing);

    ui->verticalLayout_4->addWidget(balanc_pid_chartView);
    ui->verticalLayout_4->addWidget(speed_pid_chartView);
    ui->verticalLayout_4->addWidget(speed_chartView);

    connect(balanc_pid_series, SIGNAL(hovered(QPointF, bool)), this, SLOT(balanc_pid_tipSlot(QPointF,bool)));
    connect(speed_pid_series, SIGNAL(hovered(QPointF, bool)), this, SLOT(speed_pid_tipSlot(QPointF,bool)));
    connect(speed_series, SIGNAL(hovered(QPointF, bool)), this, SLOT(speed_tipSlot(QPointF,bool)));
}

bc_upper::~bc_upper()
{
    delete ui;
}

void bc_upper::on_push_balance_p_clicked()
{
    QByteArray sendData;

    send_data.head = BC_HEAD;
    send_data.type = BC_PARAM_TYPE;
    send_data.channel = BC_BALANCE_P_PARAM_CHANNEL;
    if(ui->le_balance_p->text() == tr(""))
    {
         send_data.data = 0;
    }
    else
    {
        send_data.data = ui->le_balance_p->text().toInt();
    }
    send_data.tail = BC_TAIL;

    sendData.append((char*)&send_data, sizeof(struct data_packet));
    bc_serial.write(sendData);
}

void bc_upper::on_push_balance_i_clicked()
{
    QByteArray sendData;

    send_data.head = BC_HEAD;
    send_data.type = BC_PARAM_TYPE;
    send_data.channel = BC_BALANCE_I_PARAM_CHANNEL;
    if(ui->le_balance_i->text() == tr(""))
    {
        send_data.data = 0;
    }
    else
    {
        send_data.data = ui->le_balance_i->text().toInt();
    }
    send_data.tail = BC_TAIL;

    sendData.append((char*)&send_data, sizeof(struct data_packet));

    bc_serial.write(sendData);
}

void bc_upper::on_push_balance_d_clicked()
{
    QByteArray sendData;

    send_data.head = BC_HEAD;
    send_data.type = BC_PARAM_TYPE;
    send_data.channel = BC_BALANCE_D_PARAM_CHANNEL;
    if(ui->le_balance_d->text() == tr(""))
    {
         send_data.data = 0;
    }
    else
    {
        send_data.data = ui->le_balance_d->text().toInt();
    }
    send_data.tail = BC_TAIL;

    sendData.append((char*)&send_data, sizeof(struct data_packet));
    bc_serial.write(sendData);
}

void bc_upper::on_push_speed_p_clicked()
{
    QByteArray sendData;

    send_data.head = BC_HEAD;
    send_data.type = BC_PARAM_TYPE;
    send_data.channel = BC_SPEED_P_PARAM_CHANNEL;
    if(ui->le_speed_p->text() == tr(""))
    {
         send_data.data = 0;
    }
    else
    {
        send_data.data = ui->le_speed_p->text().toInt();
    }
    send_data.tail = BC_TAIL;

    sendData.append((char*)&send_data, sizeof(struct data_packet));
    bc_serial.write(sendData);
}

void bc_upper::on_push_speed_i_clicked()
{
    QByteArray sendData;

    send_data.head = BC_HEAD;
    send_data.type = BC_PARAM_TYPE;
    send_data.channel = BC_SPEED_I_PARAM_CHANNEL;
    if(ui->le_speed_i->text() == tr(""))
    {
         send_data.data = 0;
    }
    else
    {
        send_data.data = ui->le_speed_i->text().toInt();
    }
    send_data.tail = BC_TAIL;

    sendData.append((char*)&send_data, sizeof(struct data_packet));
    bc_serial.write(sendData);
}

void bc_upper::on_push_speed_d_clicked()
{
    QByteArray sendData;

    send_data.head = BC_HEAD;
    send_data.type = BC_PARAM_TYPE;
    send_data.channel = BC_SPEED_D_PARAM_CHANNEL;
    if(ui->le_speed_d->text() == tr(""))
    {
         send_data.data = 0;
    }
    else
    {
        send_data.data = ui->le_speed_d->text().toInt();
    }
    send_data.tail = BC_TAIL;

    sendData.append((char*)&send_data, sizeof(struct data_packet));
    bc_serial.write(sendData);
}


void bc_upper::on_push_speed_clicked()
{
    QByteArray sendData;

    send_data.head = BC_HEAD;
    send_data.type = BC_PARAM_TYPE;
    send_data.channel = BC_SPEED_PARAM_CHANNEL;
    if(ui->le_speed->text() == tr(""))
    {
         send_data.data = 0;
    }
    else
    {
        send_data.data = ui->le_speed->text().toInt();
    }
    send_data.tail = BC_TAIL;

    sendData.append((char*)&send_data, sizeof(struct data_packet));
    bc_serial.write(sendData);
}

void bc_upper::on_push_up_clicked()
{
    QByteArray sendData;

    send_data.head = BC_HEAD;
    send_data.type = BC_DIRECTION_TYPE;
    send_data.channel = BC_UP_DIRECTION_CHANNEL;
    send_data.tail = BC_TAIL;

    sendData.append((char*)&send_data, sizeof(struct data_packet));
    bc_serial.write(sendData);
}

void bc_upper::on_push_down_clicked()
{
    QByteArray sendData;

    send_data.head = BC_HEAD;
    send_data.type = BC_DIRECTION_TYPE;
    send_data.channel = BC_DOWN_DIRECTION_CHANNEL;
    send_data.tail = BC_TAIL;

    sendData.append((char*)&send_data, sizeof(struct data_packet));
    bc_serial.write(sendData);
}

void bc_upper::on_push_left_clicked()
{
    QByteArray sendData;

    send_data.head = BC_HEAD;
    send_data.type = BC_DIRECTION_TYPE;
    send_data.channel = BC_LEFT_DIRECTION_CHANNEL;
    send_data.tail = BC_TAIL;

    sendData.append((char*)&send_data, sizeof(struct data_packet));
    bc_serial.write(sendData);
}

void bc_upper::on_push_right_clicked()
{
    QByteArray sendData;

    send_data.head = BC_HEAD;
    send_data.type = BC_DIRECTION_TYPE;
    send_data.channel = BC_RIGHT_DIRECTION_CHANNEL;
    send_data.tail = BC_TAIL;

    sendData.append((char*)&send_data, sizeof(struct data_packet));
    bc_serial.write(sendData);
}

void bc_upper::on_push_left_up_clicked()
{
    QByteArray sendData;

    send_data.head = BC_HEAD;
    send_data.type = BC_DIRECTION_TYPE;
    send_data.channel = BC_LEFT_UP_DIRECTION_CHANNEL;
    send_data.tail = BC_TAIL;

    sendData.append((char*)&send_data, sizeof(struct data_packet));
    bc_serial.write(sendData);
}

void bc_upper::on_push_right_up_clicked()
{
    QByteArray sendData;

    send_data.head = BC_HEAD;
    send_data.type = BC_DIRECTION_TYPE;
    send_data.channel = BC_RIGHT_UP_DIRECTION_CHANNEL;
    send_data.tail = BC_TAIL;

    sendData.append((char*)&send_data, sizeof(struct data_packet));
    bc_serial.write(sendData);
}

void bc_upper::on_push_left_down_clicked()
{
    QByteArray sendData;

    send_data.head = BC_HEAD;
    send_data.type = BC_DIRECTION_TYPE;
    send_data.channel = BC_LEFT_DOWN_DIRECTION_CHANNEL;
    send_data.tail = BC_TAIL;

    sendData.append((char*)&send_data, sizeof(struct data_packet));
    bc_serial.write(sendData);
}

void bc_upper::on_push_right_down_clicked()
{
    QByteArray sendData;

    send_data.head = BC_HEAD;
    send_data.type = BC_DIRECTION_TYPE;
    send_data.channel = BC_RIGHT_DOWN_DIRECTION_CHANNEL;
    send_data.tail = BC_TAIL;

    sendData.append((char*)&send_data, sizeof(struct data_packet));
    bc_serial.write(sendData);
}
