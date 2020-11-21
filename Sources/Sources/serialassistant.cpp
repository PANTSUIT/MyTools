#include "serialassistant.h"
#include "ui_serialassistant.h"

SerialAssistant::SerialAssistant(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialAssistant)
{
    init_ui();  // 初始化UI
    serialPort = new QSerialPort;
    QObject::connect(serialPort,&QSerialPort::readyRead,this, &SerialAssistant::on_serialPort_readyRead);

}

SerialAssistant::~SerialAssistant()
{
    delete ui;
    delete serialPort;
}

// ui 初始化
void SerialAssistant::init_ui()
{
    ui->setupUi(this);

    ui->comboBox_portBaud->clear();
    ui->comboBox_portBaud->addItems(BaudRate_SelList);

    ui->comboBox_portParity->clear();
    ui->comboBox_portParity->addItems(Parity_SelList);

    ui->comboBox_portDatabit->clear();
    ui->comboBox_portDatabit->addItems(DataBits_SelList);

    ui->comboBox_portStopbit->clear();
    ui->comboBox_portStopbit->addItems(StopBits_SelList);
    
    ui->comboBox_portBaud->setCurrentText("115200");
    ui->comboBox_portDatabit->setCurrentText("8");
    ui->comboBox_portStopbit->setCurrentText("1");
    ui->comboBox_portParity->setCurrentText("NoParity");

    ui->radioButton_Recv_DispalyASCII->setChecked(true);
    ui->radioButton_InputASCII->setChecked(true);

    ui->label_tx_rx_number->setText(QString("TX: %1 RX: %2").arg(tx_number, 6, 10, QLatin1Char('0')).arg(rx_number, 6, 10, QLatin1Char('0')));
}

// 扫描串口
void SerialAssistant::scanSerialPort(void)
{
    this->PortNameList.clear(); // 清空原来的端口列表
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        qDebug() << info.portName() << endl;

        serialPort->setPort(info);
        if(serialPort->open(QIODevice::ReadWrite))
        {
            PortNameList.append(serialPort->portName());
            serialPort->close();
        }
    }
}

// 打开串口
bool SerialAssistant::openSerialPort(QString portName, uint32_t baudRate, QSerialPort::StopBits stopBits, QSerialPort::DataBits dataBits, QSerialPort::Parity parity)
{
    /*
    qDebug() << "PortName" << portName << endl;
    qDebug() << "baudRate" << baudRate << endl;
    qDebug() << "stopBits" << stopBits << endl;
    qDebug() << "dataBits" << dataBits << endl;
    qDebug() << "parity" << parity << endl;
    */
    serialPort->setPortName(portName);
    if(serialPort->open(QIODevice::ReadWrite))
    {
        serialPort->setBaudRate(baudRate);
        serialPort->setStopBits(stopBits);
        serialPort->setDataBits(dataBits);
        serialPort->setParity(parity);
        serialPort->setFlowControl(QSerialPort::NoFlowControl);

        this->wait_readyread_time = 1000000/baudRate;

        qDebug() << "wait out:" << wait_readyread_time << endl;

        return true;
    }
    // 打开失败，提示框
    qDebug() << "Open " << portName << " failed!" << endl;
    QMessageBox::warning(NULL,QString("Warning"),QString("Open Failed:")+ serialPort->errorString());
    return false;
}

// 发送
void SerialAssistant::on_pushButton_Send_clicked()
{
    qint32 temp = 0;

    temp = serialPort->write(inputBuffer);
    if( temp > 0)
    {
        tx_number += temp;
        qDebug() << QString("TX: %1 RX: %2").arg(tx_number, 6, 10, QLatin1Char('0')).arg(rx_number, 6, 10, QLatin1Char('0')) << endl;
    }
    qDebug() << "Write Buffer:" << inputBuffer << endl;
    qDebug() << "temp:" << temp << endl;
}

// 设置输入显示格式
void SerialAssistant::setInputFormat(Format_t format)
{
    input_DisplayFormat = format;
    if(format == Format_HEX)
    {
        ui->textEdit_Input->setText(inputBuffer.toHex(' '));
    }
    else if(format == Format_ASCII)
    {
        ui->textEdit_Input->setText(inputBuffer);
    }
}

// 获取输入显示格式
SerialAssistant::Format_t SerialAssistant::getInputFormat(void)
{
    return input_DisplayFormat;
}


// 设置接收显示格式
void SerialAssistant::setRecvFormat(Format_t format)
{
    recv_DisplayFormat = format;
    if(format == Format_HEX)
    {
        ui->textEdit_recive->setText(display_RecvBuffer.toHex(' '));
    }
    else if(format == Format_ASCII)
    {
        ui->textEdit_recive->setText(QString(display_RecvBuffer));
    }
}

// 获取接收显示格式
SerialAssistant::Format_t SerialAssistant::getRecvFormat(void)
{
    return recv_DisplayFormat;
}

// 接收串口信息
void SerialAssistant::on_serialPort_readyRead()
{
    static qint32 count = 0;

    if(flag & Flag_StopRecv)
    {
    }
    else
    {
        count++;
        qDebug() << "rady read count:" << count << endl;

        if(!serialPort->waitForReadyRead(wait_readyread_time))   // 等待接收完成，如果 10ms 内没有接收到数据，就认为接受结束
        {
            recvBuffer.clear();
            recvBuffer.append(serialPort->readAll());
            rx_number += recvBuffer.count();
            ui->label_tx_rx_number->setText(QString("TX: %1 RX: %2").arg(tx_number, 6, 10, QLatin1Char('0')).arg(rx_number, 6, 10, QLatin1Char('0')));

            if(flag & Flag_LogMode)
            {
                recvBuffer.insert(0, QString("\n[ %1 ]\n").arg(QTime::currentTime().toString()));
                //recvBuffer.append(QString("\n[ %1 ] ").arg(QTime::currentTime().toString()));
            }

            display_RecvBuffer.append(recvBuffer);

            switch(recv_DisplayFormat)
            {
                case Format_ASCII:
                    ui->textEdit_recive->setText(QString(display_RecvBuffer));
                break;
                case Format_HEX:
                    ui->textEdit_recive->setText(display_RecvBuffer.toHex(' '));
                break;
                default :
                break;
            }

//            ui->textEdit_recive->verticalScrollBar()->setValue(ui->textEdit_recive->verticalScrollBar()->maximumHeight());

            ui->textEdit_recive->moveCursor(QTextCursor::End);
        }
    }
}
// 清除输入
void SerialAssistant::clearInputBuffer(void)
{
    this->display_InputBuffer.clear();
}
// 清除接收
void SerialAssistant::clearRecvBuffer(void)
{
    this->display_RecvBuffer.clear();
    rx_number = 0;
    tx_number = 0;
}

// 清除接收
void SerialAssistant::on_pushButton_ClearRecv_clicked()
{
    this->clearRecvBuffer();
    ui->textEdit_recive->clear();
    ui->label_tx_rx_number->setText(QString("TX: %1 RX: %2").arg(tx_number, 6, 10, QLatin1Char('0')).arg(rx_number, 6, 10, QLatin1Char('0')));
}

// 清楚发送
void SerialAssistant::on_pushButton_ClearSend_clicked()
{
    this->clearInputBuffer();
    ui->textEdit_Input->clear();
}

// 扫描串口
void SerialAssistant::on_pushButton_ScanPort_clicked()
{
    this->scanSerialPort();
    ui->comboBox_portName->clear(); // 清除复选框中的内容
    ui->comboBox_portName->addItems(PortNameList);
}

// 打开/关闭 串口
void SerialAssistant::on_pushButton_OpenPort_clicked()
{
    if(ui->pushButton_OpenPort->text() == "打开")
    {
        if(openSerialPort(ui->comboBox_portName->currentText(),
                          serialPort_BaudRate[ui->comboBox_portBaud->currentText()],
                          serialPort_StopBits[ui->comboBox_portStopbit->currentText()],
                          serialPort_DataBits[ui->comboBox_portDatabit->currentText()],
                          serialPort_Parity[ui->comboBox_portParity->currentText()])
                )
        {
            ui->comboBox_portBaud->setEnabled(false);
            ui->comboBox_portName->setEnabled(false);
            ui->comboBox_portParity->setEnabled(false);
            ui->comboBox_portDatabit->setEnabled(false);
            ui->comboBox_portStopbit->setEnabled(false);

            ui->pushButton_OpenPort->setText("关闭");
        }
    }
    else
    {
        serialPort->clear();
        serialPort->close();

        ui->pushButton_OpenPort->setText("打开");
        ui->comboBox_portBaud->setEnabled(true);
        ui->comboBox_portName->setEnabled(true);
        ui->comboBox_portParity->setEnabled(true);
        ui->comboBox_portDatabit->setEnabled(true);
        ui->comboBox_portStopbit->setEnabled(true);
    }
}

void SerialAssistant::on_textEdit_Input_textChanged()
{
    QTextCursor textEdit_cursor = ui->textEdit_Input->textCursor();
    QTextLayout *pLayout = textEdit_cursor.block().layout();
    int cur_column = textEdit_cursor.position() - textEdit_cursor.block().position();
    int cur_row = pLayout->lineForTextPosition(cur_column).lineNumber() + textEdit_cursor.block().firstLineNumber();

    display_InputBuffer.clear();
    display_InputBuffer.append(ui->textEdit_Input->toPlainText());

    inputBuffer.clear();
    if(this->input_DisplayFormat == Format_HEX)
    {
        for (int i = 0; i < display_InputBuffer.count(); i++)
        {
            if(QChar(display_InputBuffer.at(i)).isSpace())
            {
            }
            else if(QChar(display_InputBuffer.at(i)).isDigit())
            {
            }
            else if(display_InputBuffer.toUpper().at(i) >= 'A' && display_InputBuffer.toUpper().at(i) <= 'F')
            {
            }
            else
            {
                QMessageBox::warning(this,"Warning","Please Enter the char 0-9 A-F a-f");

                ui->textEdit_Input->textCursor().deletePreviousChar();
                //ui->textEdit_Input->moveCursor(QTextCursor::End);
            }
         }
        inputBuffer = QByteArray::fromHex(display_InputBuffer);
    }
    else
    {
        inputBuffer = display_InputBuffer;
    }

    qDebug() << "cur_row:" << cur_row << endl;
    qDebug() << "cur_column:" << cur_column << endl;
    qDebug() << "display_InputBuffer" << display_InputBuffer << endl;
    qDebug() << "inputBuffer" << inputBuffer << endl;

    qDebug() << "Block Number:" << ui->textEdit_Input->textCursor().blockNumber() << endl;

    qDebug() << "column Number:" << ui->textEdit_Input->textCursor().columnNumber() << endl;
}

void SerialAssistant::on_radioButton_Recv_DisplayHEX_clicked(bool checked)
{
    if(checked)
    {
        this->setRecvFormat(Format_HEX);
        qDebug() << "Set Recv display format to hex " << endl;
    }
}

void SerialAssistant::on_radioButton_Recv_DispalyASCII_clicked(bool checked)
{
    if(checked)
    {
        this->setRecvFormat(Format_ASCII);
        qDebug() << "Set Recv display format to ASCII " << endl;
    }
}

void SerialAssistant::on_radioButton_InputHEX_clicked(bool checked)
{
    if(checked)
    {
        this->setInputFormat(Format_HEX);
        qDebug() << "Set Input display format to hex " << endl;
    }
}

void SerialAssistant::on_radioButton_InputASCII_clicked(bool checked)
{
    if(checked)
    {
        this->setInputFormat(Format_ASCII);
        qDebug() << "Set Input display format to ASCII " << endl;
    }
}

void SerialAssistant::on_checkBox_StopRecv_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked)
    {
        flag |= Flag_StopRecv;
    }
    else
    {
        flag &= ~Flag_StopRecv;
    }
}

void SerialAssistant::on_checkBox_LogMode_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked)
    {
        flag |= Flag_LogMode;
    }
    else
    {
        flag &= ~Flag_LogMode;
    }
}
