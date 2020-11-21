#ifndef SERIALASSISTANT_H
#define SERIALASSISTANT_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QDebug>
#include <QThread>
#include <QTextEdit>
#include <QTextBlock>
#include <QTime>
#include <QDate>

#include <QTextDocument>

#include <QScrollBar>
#include <QString>
#include <QMap>


namespace Ui {
class SerialAssistant;
}

class SerialAssistant : public QWidget {
    Q_OBJECT

public:

    typedef enum
    {
        Format_ASCII,
        Format_HEX,
    }Format_t;

    typedef enum
    {
        Flag_StopRecv = 0x01 << 0,
        Flag_LogMode =  0x01 << 1,
    }FunctionFlag;

    Format_t input_DisplayFormat = Format_ASCII,recv_DisplayFormat = Format_ASCII;


    QHash<QString, QSerialPort::BaudRate> serialPort_BaudRate = {
        // BaudRate
        {"1200",QSerialPort::Baud1200},
        {"2400",QSerialPort::Baud2400},
        {"4800",QSerialPort::Baud4800},
        {"9600",QSerialPort::Baud9600},
        {"19200",QSerialPort::Baud19200},
        {"38400",QSerialPort::Baud38400},
        {"57600",QSerialPort::Baud57600},
        {"115200",QSerialPort::Baud115200},
    };

    QHash<QString, QSerialPort::StopBits> serialPort_StopBits = {
        {"1",QSerialPort::OneStop},
        {"1.5",QSerialPort::OneAndHalfStop},
        {"2",QSerialPort::TwoStop}
    };

    QHash<QString, QSerialPort::DataBits> serialPort_DataBits = {
        {"5", QSerialPort::Data5},
        {"6", QSerialPort::Data6},
        {"7", QSerialPort::Data7},
        {"8", QSerialPort::Data8},
    };

    QHash<QString, QSerialPort::Parity> serialPort_Parity = {
        {"NoParity",QSerialPort::NoParity},
        {"EvenParity",QSerialPort::EvenParity},
        {"OddParity",QSerialPort::OddParity},
        {"SpaceParity",QSerialPort::SpaceParity},
        {"MarkParity",QSerialPort::MarkParity},
    };
    QHash<QString,QSerialPort::FlowControl> serialPort_FlowControl = {
        {"NoFlowControl",QSerialPort::NoFlowControl},
        {"HardwareControl",QSerialPort::HardwareControl},
        {"SoftwareControl",QSerialPort::SoftwareControl},
    };

    QSerialPort *serialPort;
    QStringList PortNameList;   // 扫描后可用的端口名称列表

    QStringList BaudRate_SelList = {"1200","2400","4800","9600","14400","19200","38400","57600","115200"};
    QStringList StopBits_SelList = {"1","1.5","2"};
    QStringList DataBits_SelList = {"5","6","7","8"};
    QStringList Parity_SelList = {"NoParity","EvenParity","OddParity","SpaceParity","MarkParity"};
    QStringList FlowControl_SelList = {"NoFlowControl","HardwareControl","SoftwareControl"};

    qint8 flag = 0;

    qint32 wait_readyread_time = 0;
    qint32 rx_number= 0, tx_number = 0;
    QByteArray recvBuffer;      // 每一次接收的数据
    QByteArray inputBuffer;     // 每一次输入的数据
    QByteArray display_InputBuffer;     // 显示输入的数据
    QByteArray display_RecvBuffer;      // 显示接收的数据

    void init_ui(); // Init the GUI

    void scanSerialPort();    // 扫面端口

    bool openSerialPort(QString portName,    
                        uint32_t baudRate = 9600, 
                        QSerialPort::StopBits stopBits = QSerialPort::OneStop,
                        QSerialPort::DataBits dataBits = QSerialPort::Data8,
                        QSerialPort::Parity   parity = QSerialPort::NoParity);


    void setInputFormat(Format_t format);  // 设置发送显示16进制
    Format_t getInputFormat(void);  // 获取发送显示格式

    void setRecvFormat(Format_t format);  // 设置发送显示16进制
    Format_t getRecvFormat(void);   // 获取接收显示格式

    void clearInputBuffer();
    void clearRecvBuffer();

    explicit SerialAssistant(QWidget *parent = nullptr);
    ~SerialAssistant();

private slots:

    void on_serialPort_readyRead();

    void on_pushButton_ClearRecv_clicked();

    void on_pushButton_ScanPort_clicked();

    void on_pushButton_OpenPort_clicked();

    void on_pushButton_Send_clicked();

    void on_textEdit_Input_textChanged();

    void on_radioButton_Recv_DisplayHEX_clicked(bool checked);

    void on_radioButton_Recv_DispalyASCII_clicked(bool checked);

    void on_radioButton_InputHEX_clicked(bool checked);

    void on_radioButton_InputASCII_clicked(bool checked);

    void on_pushButton_ClearSend_clicked();

    void on_checkBox_StopRecv_stateChanged(int arg1);

    void on_checkBox_LogMode_stateChanged(int arg1);

private:
    Ui::SerialAssistant *ui;
};

#endif // SERIALASSISTANT_H
