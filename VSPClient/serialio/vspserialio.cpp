#include "ui_vspserialio.h"
#include <QDebug>
#include <QScreen>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>
#include <QTimer>
#include <vspserialio.h>

Q_DECLARE_METATYPE(QSerialPortInfo)
Q_DECLARE_METATYPE(QSerialPort::BaudRate)
Q_DECLARE_METATYPE(QSerialPort::DataBits)
Q_DECLARE_METATYPE(QSerialPort::StopBits)
Q_DECLARE_METATYPE(QSerialPort::Parity)

VSPSerialIO::VSPSerialIO(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::VSPSerialIO)
    , m_port(nullptr)
    , m_looper(nullptr)
    , m_outTotal(0)
    , m_isLooping(false)
    , m_looperStop(false)
    , m_looperCount(0)
{
    ui->setupUi(this);

    ui->gbxOutput->setEnabled(false);

    initComboSerialPort(ui->cbxComPort, nullptr);
    initComboBaudRate(ui->cbxBaud, nullptr);
    initComboDataBits(ui->cbxDataBits, nullptr);
    initComboStopBits(ui->cbxStopBits, nullptr);
    initComboParity(ui->cbxParity, nullptr);
    initComboFlowCtrl(ui->cbxFlowControl, nullptr);

    ui->cbxDtr->setChecked(false);
    if (!ui->cbxDtr->property("init").toBool()) {
        ui->cbxDtr->setProperty("init", QVariant::fromValue(true));
        connect(ui->cbxDtr, qOverload<bool>(&QCheckBox::clicked), this, [this](bool state) {
            // TODO: onClickEvent()
            Q_UNUSED(this)
            Q_UNUSED(state)
        });
    }
    ui->cbxRts->setChecked(false);
    if (!ui->cbxRts->property("init").toBool()) {
        ui->cbxRts->setProperty("init", QVariant::fromValue(true));
        connect(ui->cbxRts, qOverload<bool>(&QCheckBox::clicked), this, [this](bool state) {
            // TODO: onClickEvent()
            Q_UNUSED(this)
            Q_UNUSED(state)
        });
    }
    ui->cbxCts->setChecked(false);
    if (!ui->cbxCts->property("init").toBool()) {
        ui->cbxCts->setProperty("init", QVariant::fromValue(true));
        connect(ui->cbxCts, qOverload<bool>(&QCheckBox::clicked), this, [this](bool state) {
            // TODO: onClickEvent()
            Q_UNUSED(this)
            Q_UNUSED(state)
        });
    }
    ui->cbxDSR->setChecked(false);
    if (!ui->cbxDSR->property("init").toBool()) {
        ui->cbxDSR->setProperty("init", QVariant::fromValue(true));
        connect(ui->cbxDSR, qOverload<bool>(&QCheckBox::clicked), this, [this](bool state) {
            // TODO: onClickEvent()
            Q_UNUSED(this)
            Q_UNUSED(state)
        });
    }

    connect(qApp, &QGuiApplication::aboutToQuit, this, [this]() {
        disconnectPort();
    });

    connect(qApp, &QGuiApplication::lastWindowClosed, this, []() {
        qApp->quit();
    });
}

VSPSerialIO::~VSPSerialIO()
{
    delete ui;
}

inline void VSPSerialIO::initComboSerialPort(QComboBox* cbx, QComboBox* link)
{
    const QIcon icon1(":/assets/png/vspclient_1.png");
    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    const int selection = (!link ? 0 : link->currentIndex());
    const QVariant vsel = (!link ? QVariant() : link->itemData(selection));

    /* fill combo box with serial ports */
    cbx->clear();
    cbx->setCurrentIndex(-1);
    for (int i = 0; i < ports.count(); i++) {
        // <Name>, <QSerialPortInfo>
        cbx->addItem(icon1, ports[i].portName(), QVariant::fromValue(ports[i]));
        if (!vsel.isNull() && vsel.isValid()) {
            if (ports[i].portName().contains(vsel.toString())) {
                cbx->setCurrentIndex(i);
            }
        }
    }
    if (cbx->currentIndex() < 0) {
        cbx->setCurrentIndex(0);
    }

    /* if called more skip -> connect event only once */
    if (cbx->property("init").toBool()) {
        return;
    }

    cbx->setProperty("init", true);
    connect(cbx, qOverload<int>(&QComboBox::activated), this, [this, cbx, link](int index) {
        const QSerialPortInfo spi = cbx->itemData(index).value<QSerialPortInfo>();
        const int selection = (!link ? 0 : link->currentIndex());
        const QVariant vsel = (!link ? QVariant() : link->itemData(selection));
        Q_UNUSED(this)
        Q_UNUSED(cbx)
        Q_UNUSED(link)
        Q_UNUSED(index)
        Q_UNUSED(spi)
        Q_UNUSED(selection)
        Q_UNUSED(vsel)
    });
}

inline void VSPSerialIO::initComboBaudRate(QComboBox* cbx, QComboBox* link)
{
    const QIcon icon3(":/res/actions/16x16/debug-exec.png");

    typedef struct {
        QString name;
        QSerialPort::BaudRate baud;
    } TBaudRates;

    TBaudRates baudRates[8] = {
       {tr("1200"), QSerialPort::Baud1200},
       {tr("2400"), QSerialPort::Baud2400},
       {tr("4800"), QSerialPort::Baud4800},
       {tr("9600"), QSerialPort::Baud9600},
       {tr("19200"), QSerialPort::Baud19200},
       {tr("38400"), QSerialPort::Baud38400},
       {tr("57600"), QSerialPort::Baud57600},
       {tr("115200"), QSerialPort::Baud115200},
    };

    const int selection = (!link ? 0 : link->currentIndex());
    QVariant vsel = (!link ? QVariant() : link->itemData(selection));

    if (vsel.isNull() || !vsel.isValid()) {
        vsel = QVariant::fromValue(baudRates[7].baud);
    }

    cbx->clear();
    for (int i = 0; i < 8; i++) {
        cbx->addItem(icon3, baudRates[i].name, QVariant::fromValue(baudRates[i].baud));
        if (!vsel.isNull() && vsel.isValid()) {
            if (baudRates[i].baud == vsel.value<QSerialPort::BaudRate>()) {
                cbx->setCurrentIndex(i);
            }
        }
    }

    /* connect event only once */
    if (cbx->property("init").toBool()) {
        return;
    }

    cbx->setProperty("init", true);
    connect(cbx, qOverload<int>(&QComboBox::activated), this, [this, cbx, link](int index) {
        const QSerialPort::BaudRate value = cbx->itemData(index).value<QSerialPort::BaudRate>();
        const int selection = (!link ? 0 : link->currentIndex());
        const QVariant vsel = (!link ? QVariant() : link->itemData(selection));
        Q_UNUSED(this)
        Q_UNUSED(cbx)
        Q_UNUSED(link)
        Q_UNUSED(index)
        Q_UNUSED(selection)
        Q_UNUSED(vsel)
        Q_UNUSED(value)
    });
}

inline void VSPSerialIO::initComboDataBits(QComboBox* cbx, QComboBox* link)
{
    const QIcon icon3(":/res/actions/16x16/debug-exec.png");

    typedef struct {
        QString name;
        QSerialPort::DataBits bits;
    } TDataBits;

    TDataBits dataBits[4] = {
       {tr("5 Bits"), QSerialPort::Data5},
       {tr("6 Bits"), QSerialPort::Data6},
       {tr("7 Bits"), QSerialPort::Data7},
       {tr("8 Bits"), QSerialPort::Data8},
    };

    const int selection = (!link ? 0 : link->currentIndex());
    QVariant vsel = (!link ? QVariant() : link->itemData(selection));

    if (vsel.isNull() || !vsel.isValid()) {
        vsel = QVariant::fromValue(dataBits[3].bits);
    }

    cbx->clear();
    for (int i = 0; i < 4; i++) {
        cbx->addItem(icon3, dataBits[i].name, QVariant::fromValue(dataBits[i].bits));
        if (!vsel.isNull() && vsel.isValid()) {
            if (dataBits[i].bits == vsel.value<QSerialPort::DataBits>()) {
                cbx->setCurrentIndex(i);
            }
        }
    }

    /* connect event only once */
    if (cbx->property("init").toBool()) {
        return;
    }

    cbx->setProperty("init", true);
    connect(cbx, qOverload<int>(&QComboBox::activated), this, [this, cbx, link](int index) {
        const QSerialPort::DataBits value = cbx->itemData(index).value<QSerialPort::DataBits>();
        const int selection = (!link ? 0 : link->currentIndex());
        const QVariant vsel = (!link ? QVariant() : link->itemData(selection));
        Q_UNUSED(this)
        Q_UNUSED(cbx)
        Q_UNUSED(link)
        Q_UNUSED(index)
        Q_UNUSED(selection)
        Q_UNUSED(vsel)
        Q_UNUSED(value)
    });
}

inline void VSPSerialIO::initComboStopBits(QComboBox* cbx, QComboBox* link)
{
    const QIcon icon3(":/res/actions/16x16/debug-exec.png");

    typedef struct {
        QString name;
        QSerialPort::StopBits bits;
    } TStopBits;

    TStopBits stopBits[3] = {
       {tr("One Stop"), QSerialPort::OneStop},
       {tr("One and Half"), QSerialPort::OneAndHalfStop},
       {tr("Two Stop"), QSerialPort::TwoStop},
    };

    const int selection = (!link ? 0 : link->currentIndex());
    QVariant vsel = (!link ? QVariant() : link->itemData(selection));

    if (vsel.isNull() || !vsel.isValid()) {
        vsel = QVariant::fromValue(stopBits[0].bits);
    }

    cbx->clear();
    for (int i = 0; i < 3; i++) {
        cbx->addItem(icon3, stopBits[i].name, QVariant::fromValue(stopBits[i].bits));
        if (!vsel.isNull() && vsel.isValid()) {
            if (stopBits[i].bits == vsel.value<QSerialPort::StopBits>()) {
                cbx->setCurrentIndex(i);
            }
        }
    }

    /* connect event only once */
    if (cbx->property("init").toBool()) {
        return;
    }

    cbx->setProperty("init", true);
    connect(cbx, qOverload<int>(&QComboBox::activated), this, [this, cbx, link](int index) {
        const QSerialPort::StopBits value = cbx->itemData(index).value<QSerialPort::StopBits>();
        const int selection = (!link ? 0 : link->currentIndex());
        const QVariant vsel = (!link ? QVariant() : link->itemData(selection));
        Q_UNUSED(this)
        Q_UNUSED(cbx)
        Q_UNUSED(link)
        Q_UNUSED(index)
        Q_UNUSED(selection)
        Q_UNUSED(vsel)
        Q_UNUSED(value)
    });
}

inline void VSPSerialIO::initComboParity(QComboBox* cbx, QComboBox* link)
{
    const QIcon icon3(":/res/actions/16x16/debug-exec.png");

    typedef struct {
        QString name;
        QSerialPort::Parity parity;
    } TParity;

    TParity parity[5] = {
       {tr("No Parity"), QSerialPort::NoParity},
       {tr("Event Parity"), QSerialPort::EvenParity},
       {tr("Odd Parity"), QSerialPort::OddParity},
       {tr("Space Parity"), QSerialPort::SpaceParity},
       {tr("Mark Parity"), QSerialPort::MarkParity},
    };

    const int selection = (!link ? 0 : link->currentIndex());
    QVariant vsel = (!link ? QVariant() : link->itemData(selection));

    if (vsel.isNull() || !vsel.isValid()) {
        vsel = QVariant::fromValue(parity[0].parity);
    }

    cbx->clear();
    for (int i = 0; i < 5; i++) {
        cbx->addItem(icon3, parity[i].name, QVariant::fromValue(parity[i].parity));
        if (!vsel.isNull() && vsel.isValid()) {
            if (parity[i].parity == vsel.value<QSerialPort::Parity>()) {
                cbx->setCurrentIndex(i);
            }
        }
    }

    /* connect event only once */
    if (cbx->property("init").toBool()) {
        return;
    }

    cbx->setProperty("init", true);
    connect(cbx, qOverload<int>(&QComboBox::activated), this, [this, cbx, link](int index) {
        const QSerialPort::Parity value = cbx->itemData(index).value<QSerialPort::Parity>();
        const int selection = (!link ? 0 : link->currentIndex());
        const QVariant vsel = (!link ? QVariant() : link->itemData(selection));
        Q_UNUSED(this)
        Q_UNUSED(cbx)
        Q_UNUSED(link)
        Q_UNUSED(index)
        Q_UNUSED(selection)
        Q_UNUSED(vsel)
        Q_UNUSED(value)
    });
}

inline void VSPSerialIO::initComboFlowCtrl(QComboBox* cbx, QComboBox* link)
{
    const QIcon icon3(":/res/actions/16x16/debug-exec.png");

    typedef struct {
        QString name;
        QSerialPort::FlowControl flow;
    } TFlowControl;

    TFlowControl flowctrl[3] = {
       {tr("No Flow Control"), QSerialPort::NoFlowControl},
       {tr("Hardware Control"), QSerialPort::HardwareControl},
       {tr("Software Control"), QSerialPort::SoftwareControl},
    };

    const int selection = (!link ? 0 : link->currentIndex());
    QVariant vsel = (!link ? QVariant() : link->itemData(selection));

    if (vsel.isNull() || !vsel.isValid()) {
        vsel = QVariant::fromValue(flowctrl[0].flow);
    }

    cbx->clear();
    for (int i = 0; i < 3; i++) {
        cbx->addItem(icon3, flowctrl[i].name, QVariant::fromValue(flowctrl[i].flow));
        if (!vsel.isNull() && vsel.isValid()) {
            if (flowctrl[i].flow == vsel.value<QSerialPort::FlowControl>()) {
                cbx->setCurrentIndex(i);
            }
        }
    }

    /* connect event only once */
    if (cbx->property("init").toBool()) {
        return;
    }

    cbx->setProperty("init", true);
    connect(cbx, qOverload<int>(&QComboBox::activated), this, [this, cbx, link](int index) {
        const QSerialPort::FlowControl value = cbx->itemData(index).value<QSerialPort::FlowControl>();
        const int selection = (!link ? 0 : link->currentIndex());
        const QVariant vsel = (!link ? QVariant() : link->itemData(selection));
        Q_UNUSED(this)
        Q_UNUSED(cbx)
        Q_UNUSED(link)
        Q_UNUSED(index)
        Q_UNUSED(selection)
        Q_UNUSED(vsel)
        Q_UNUSED(value)
    });
}

void VSPSerialIO::on_btnSendFile_clicked()
{
}

inline void VSPSerialIO::connectPort()
{
    if (!m_port->isOpen()) {
        if (!m_port->open(QSerialPort::ReadWrite)) {
            ui->txInputView->setPlainText("Unable to connect serial port " + m_port->portName());
            return;
        }

        ui->btnConnect->setText("Disconnect");
        ui->gbxOutput->setEnabled(true);

        m_port->flush();
    }
}

inline void VSPSerialIO::disconnectPort()
{
    if (m_port && m_port->isOpen()) {
        ui->btnConnect->setText("Connect");
        ui->gbxOutput->setEnabled(false);

        if (m_looper) {
            m_looperStop = true;
        }

        m_isLooping = false;
        ui->btnLooper->setText("Looper");
        ui->btnLooper->setEnabled(true);

        m_port->flush();
        m_port->close();
    }
}

void VSPSerialIO::on_btnConnect_clicked()
{
    if (!m_port) {
        m_port = new QSerialPort(ui->cbxComPort->currentData().value<QSerialPortInfo>(), this);
        m_port->setDataBits(ui->cbxDataBits->currentData().value<QSerialPort::DataBits>());
        m_port->setStopBits(ui->cbxStopBits->currentData().value<QSerialPort::StopBits>());
        m_port->setParity(ui->cbxParity->currentData().value<QSerialPort::Parity>());
        m_port->setFlowControl(ui->cbxFlowControl->currentData().value<QSerialPort::FlowControl>());
        connect(m_port, &QSerialPort::errorOccurred, this, &VSPSerialIO::onPortErrorOccured);
        connect(m_port, &QSerialPort::bytesWritten, this, &VSPSerialIO::onPortBytesWritten);
        connect(m_port, &QSerialPort::aboutToClose, this, &VSPSerialIO::onPortClosed);
        connect(m_port, &QSerialPort::readyRead, this, &VSPSerialIO::onPortReadyRead);
        connect(m_port, &QSerialPort::destroyed, this, [this](QObject*) {
            m_port = nullptr;
        });
    }

    if (!m_port->isOpen()) {
        connectPort();
    }
    else {
        disconnectPort();
    }
}

void VSPSerialIO::on_edOutputLine_textEdited(const QString&)
{
}

void VSPSerialIO::on_btnSendLine_clicked()
{
    if (!m_port)
        return;

    QByteArray endings[7] = {"\r\n", "\n", "\r", "$", "|", ":", ";"};
    QByteArray out = ui->edOutputLine->text().toUtf8();

    int index = ui->cbxLineEnding->currentIndex();
    if (index > -1 && index < 7) {
        out.append(endings[index]);
    }

    if (m_port && m_port->isOpen()) {
        ui->txInputView->setPlainText(">: " + out);

        qDebug() << "VSPTester: SND:" << out.toHex().constData();

        if (m_port->write(out) != out.length()) {
            m_port->close();
        }
    }
}

void VSPSerialIO::looperLooper()
{
    const QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

    if (m_looperStop) {
        return;
    }

    if (m_looperCount >= chars.length()) {
        m_looperCount = 0;
    }

    QString dummy;
    for (int i = 0; i < ui->edGenLength->value(); i++) {
        dummy += chars.at(m_looperCount);
    }

    ui->edOutputLine->setText(dummy);

    m_looper = new QThread();
    connect(m_looper, &QThread::started, this, [this]() {
        qDebug() << "th started";
        QTimer::singleShot(1000, this, [this]() {
            on_btnSendLine_clicked();
            m_looperCount++;
            looperLooper();
        });
        m_looper->exit(0);
    });
    connect(m_looper, &QThread::finished, this, [this]() {
        qDebug() << "th finished";
        m_looper->deleteLater();
    });
    connect(m_looper, &QSerialPort::destroyed, this, [](QObject*) {
        qDebug() << "th removed";
    });
    m_looper->setObjectName("looper");
    m_looper->start(QThread::NormalPriority);
}

void VSPSerialIO::on_btnLooper_clicked()
{
    if (!m_isLooping) {
        ui->btnLooper->setText("Looping");
        ui->btnLooper->setChecked(true);
        m_looperCount = 0;
        m_looperStop = false;
        m_isLooping = true;
        looperLooper();
    }
    else {
        ui->btnLooper->setText("Looper");
        ui->btnLooper->setChecked(false);
        m_isLooping = false;
        m_looperStop = true;
    }
}

void VSPSerialIO::onPortErrorOccured(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError) {
        QTimer::singleShot(100, this, [this, error]() {
            QString msg = QStringLiteral("Serial port error: %1").arg(error);
            ui->txInputView->setPlainText(msg);
        });
    }
}

void VSPSerialIO::onPortBytesWritten(qint64 bytes)
{
    m_outTotal += bytes;

    ui->txOutputInfo->setText(QStringLiteral( //
                                 "Written: %1 Total: %2")
                                 .arg(bytes)
                                 .arg(m_outTotal));

    qDebug() << "VSPTester:" << ui->txOutputInfo->text().toUtf8().constData();
}

void VSPSerialIO::onPortReadyRead()
{
    if (!m_port)
        return;

    QByteArray inbuf = m_port->readAll();

    qDebug() << "VSPTester: RCV:" << inbuf.toHex().constData();

    QString buffer = ui->txInputView->toPlainText();
    buffer += "<: " + inbuf;
    ui->txInputView->setPlainText(buffer);
}

void VSPSerialIO::onPortClosed()
{
    m_port->deleteLater();
    m_port = nullptr;

    if (m_looper) {
        m_looperStop = true;
    }

    ui->btnConnect->setText("Connect");
    ui->btnConnect->setEnabled(true);
    ui->gbxOutput->setEnabled(false);

    m_isLooping = false;
    ui->btnLooper->setText("Looper");
    ui->btnLooper->setEnabled(true);
}

void VSPSerialIO::on_actionNewWindow_triggered()
{
    VSPSerialIO* w = new VSPSerialIO();
    w->show();
}
