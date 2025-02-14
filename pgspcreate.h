#pragma once

#include "vspdatamodel.h"
#include <QComboBox>
#include <QPushButton>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QWidget>

namespace Ui {
class PGSPCreate;
}

class PGSPCreate: public QWidget
{
    Q_OBJECT

public:
    explicit PGSPCreate(QWidget* parent = nullptr);
    ~PGSPCreate();
    QPushButton* button();
    void setModel(VSPDataModel* model);
    QSerialPort::BaudRate baudRate() const;
    QSerialPort::DataBits dataBits() const;
    QSerialPort::StopBits stopBits() const;
    QSerialPort::Parity parity() const;
    QSerialPort::FlowControl flowCtrl() const;

private:
    Ui::PGSPCreate* ui;

private:
    inline void initComboSerialPort(QComboBox* cbx, QComboBox* link = nullptr);
    inline void initComboBaudRate(QComboBox* cbx, QComboBox* link = nullptr);
    inline void initComboDataBits(QComboBox* cbx, QComboBox* link = nullptr);
    inline void initComboStopBits(QComboBox* cbx, QComboBox* link = nullptr);
    inline void initComboParity(QComboBox* cbx, QComboBox* link = nullptr);
    inline void initComboFlowCtrl(QComboBox* cbx, QComboBox* link = nullptr);
};
