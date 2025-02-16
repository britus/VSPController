#include "ui_vscmainwindow.h"
#include <QDebug>
#include <QDesktopServices>
#include <QMovie>
#include <QTimer>
#include <vscmainwindow.h>

VSCMainWindow::VSCMainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::VSCMainWindow)
    , m_vsp(nullptr)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->pg09Connect);

    m_vsp = new VSPDriverClient(this);
    connect(m_vsp, &VSPDriverClient::didFailWithError, this, &VSCMainWindow::onSetupFailWithError);
    connect(m_vsp, &VSPDriverClient::didFinishWithResult, this, &VSCMainWindow::onSetupFinishWithResult);
    connect(m_vsp, &VSPDriverClient::needsUserApproval, this, &VSCMainWindow::onSetupNeedsUserApproval);
    connect(m_vsp, &VSPDriverClient::connected, this, &VSCMainWindow::onClientConnected);
    connect(m_vsp, &VSPDriverClient::disconnected, this, &VSCMainWindow::onClientDisconnected);
    connect(m_vsp, &VSPDriverClient::errorOccured, this, &VSCMainWindow::onClientError);
    connect(m_vsp, &VSPDriverClient::updateStatusLog, this, &VSCMainWindow::onUpdateStatusLog);
    connect(m_vsp, &VSPDriverClient::updateButtons, this, &VSCMainWindow::onUpdateButtons);
    connect(m_vsp, &VSPDriverClient::complete, this, &VSCMainWindow::onComplete);

    connect(ui->pg01SPCreate->button(), &QPushButton::clicked, this, &VSCMainWindow::onActionSPCreate);
    connect(ui->pg02SPRemove->button(), &QPushButton::clicked, this, &VSCMainWindow::onActionSPRemove);
    connect(ui->pg03LKCreate->button(), &QPushButton::clicked, this, &VSCMainWindow::onActionLKCreate);
    connect(ui->pg04LKRemove->button(), &QPushButton::clicked, this, &VSCMainWindow::onActionLKRemove);
    connect(ui->pg05PortList->button(), &QPushButton::clicked, this, &VSCMainWindow::onActionPortList);
    connect(ui->pg06LinkList->button(), &QPushButton::clicked, this, &VSCMainWindow::onActionLinkList);
    connect(ui->pg07Checks->button(), &QPushButton::clicked, this, &VSCMainWindow::onActionEditChecks);
    connect(ui->pg08Traces->button(), &QPushButton::clicked, this, &VSCMainWindow::onActionEditTraces);
    connect(ui->pg09Connect->button(), &QPushButton::clicked, this, &VSCMainWindow::onActionVspConnect);

#ifdef QT_DEBUG
    ui->btn08Traces->setVisible(true);
#else
    ui->btn08Traces->setVisible(false);
#endif

    disableButtonState(
       QList<QPushButton*>() //
       << ui->btn01SPCreate  //
       << ui->btn02SPRemove  //
       << ui->btn03LKCreate  //
       << ui->btn04LKRemove  //
       << ui->btn05PortList  //
       << ui->btn06LinkList  //
       << ui->btn07Checks    //
       << ui->btn08Traces    //
       << ui->btn09Connect);

    onActionVspConnect();
}

VSCMainWindow::~VSCMainWindow()
{
    delete m_vsp;
    delete ui;
}

inline void VSCMainWindow::showOverlay()
{
    QWidget* overlay;
    QLabel* gifLabel;

    // Overlay-Widget erstellen
    overlay = new QWidget(centralWidget());

    // Mausereignisse durchlassen
    overlay->setAttribute(Qt::WA_TransparentForMouseEvents);

    // 80% transparent
    overlay->setStyleSheet( //
       "background-color: rgba(0, 0, 0, 51); "
       "border-color: rgb(252, 115, 9); "
       "border-style: solid; "
       "border-width: 1px; "
       "border-radius: 5px;");
    setProperty("overlay", QVariant::fromValue(overlay));

    // GIF-Label erstellen
    gifLabel = new QLabel(overlay);
    QMovie* movie = new QMovie(":/progress1", QByteArray(), this);
    gifLabel->setMovie(movie);
    movie->start();

    // Overlay-Größe anpassen
    QTimer* resizer = new QTimer(this);
    connect(resizer, &QTimer::timeout, this, &VSCMainWindow::updateOverlayGeometry);
    resizer->setSingleShot(true);
    resizer->start(20);

    if (ui->stackedWidget->currentWidget() != ui->pg09Connect) {
        ui->pnlButtons->setEnabled(false);
        ui->pnlContent->setEnabled(false);
    }
}

inline void VSCMainWindow::removeOverlay()
{
    QVariant v = property("overlay");
    if (v.isNull() || !v.isValid()) {
        return;
    }

    QWidget* overlay;
    if (!(overlay = v.value<QWidget*>())) {
        return;
    }

    setProperty("overlay", {});
    overlay->hide();
    overlay->deleteLater();
}

void VSCMainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    updateOverlayGeometry();
}

void VSCMainWindow::onSetupFailWithError(uint32_t code, const char* message)
{
    ui->textBrowser->setPlainText(tr("VSP setup status #%1\nInfo: %2") //
                                     .arg(code)
                                     .arg(message));
}

void VSCMainWindow::onSetupFinishWithResult(uint32_t code, const char* message)
{
    ui->textBrowser->setPlainText(tr("%1 %2").arg(code).arg(message));
}

void VSCMainWindow::onSetupNeedsUserApproval()
{
}

void VSCMainWindow::onClientConnected()
{
    qDebug("CTRLWIN::onClientConnected()\n");
    ui->stackedWidget->setCurrentWidget(ui->pg01SPCreate);
    enableButtonState(
       QList<QPushButton*>() //
       << ui->btn01SPCreate  //
       << ui->btn02SPRemove  //
       << ui->btn03LKCreate  //
       << ui->btn04LKRemove  //
       << ui->btn05PortList  //
       << ui->btn06LinkList  //
       << ui->btn07Checks    //
       << ui->btn08Traces);
    on_btn01SPCreate_clicked();
    disableButtonState(ui->btn09Connect);
    ui->textBrowser->setPlainText("Connected.");
    removeOverlay();
}

void VSCMainWindow::onClientDisconnected()
{
    qDebug("CTRLWIN::onClientDisconnected()\n");
    ui->stackedWidget->setCurrentWidget(ui->pg09Connect);
    disableButtonState(
       QList<QPushButton*>() //
       << ui->btn01SPCreate  //
       << ui->btn02SPRemove  //
       << ui->btn03LKCreate  //
       << ui->btn04LKRemove  //
       << ui->btn05PortList  //
       << ui->btn06LinkList  //
       << ui->btn07Checks    //
       << ui->btn08Traces);
    enableButtonState(ui->btn09Connect);
    enableDefaultButton(ui->btn09Connect);
    ui->textBrowser->setPlainText("Disconnected.");
}

void VSCMainWindow::onClientError(int error, const QString& message)
{
    QString text;

    text += QStringLiteral("VSP driver error %1: %2\n\n").arg(error).arg(message);
    if (!m_vsp->IsConnected() && error == kIOErrorNotFound) {
        text += "You must install the VSP Driver extension first.\n";
    }

    ui->textBrowser->setPlainText(text);

    if (ui->stackedWidget->currentWidget() != ui->pg09Connect) {
        ui->stackedWidget->currentWidget()->setEnabled(false);
    }

    removeOverlay();
}

void VSCMainWindow::onUpdateStatusLog(const QByteArray& message)
{
    ui->textBrowser->setPlainText(message);
}

void VSCMainWindow::onUpdateButtons(bool enabled)
{
    ui->pnlButtons->setEnabled(enabled);
    ui->pnlContent->setEnabled(enabled);
}

void VSCMainWindow::onComplete()
{
    ui->stackedWidget->currentWidget()->setEnabled(true);
    removeOverlay();
}

void VSCMainWindow::updateOverlayGeometry()
{
    QVariant v = property("overlay");
    if (v.isNull() || !v.isValid()) {
        return;
    }

    QWidget* overlay;
    if (!(overlay = v.value<QWidget*>())) {
        return;
    }

    QLabel* gifLabel;
    if (overlay->children().isEmpty() || !(gifLabel = dynamic_cast<QLabel*>(overlay->children().at(0)))) {
        return;
    }

    if (centralWidget()) {
        QRect ovr = centralWidget()->rect();
        int width = 100;  // ovr.width() - (ovr.width() * 15 / 100);
        int height = 100; // ovr.height() - (ovr.height() * 15 / 100);
        int x = ovr.width() / 2 - (width / 2);
        int y = ovr.height() / 2 - (height / 2);

        ovr.setTopLeft(QPoint(x, y));
        ovr.setSize(QSize(width, height));

        // Overlay an zentrale Widget-Größe anpassen
        overlay->setGeometry(ovr);

        // GIF zentrieren und skalieren
        gifLabel->setGeometry(overlay->rect().adjusted(2, 2, -2, -2));
        gifLabel->setAlignment(Qt::AlignCenter);
        gifLabel->movie()->setScaledSize(gifLabel->size());
    }

    QTimer* t;
    if ((t = dynamic_cast<QTimer*>(sender()))) {
        overlay->show();
        if (t->isActive()) {
            t->stop();
        }
        t->deleteLater();
    }
}

inline void VSCMainWindow::resetDefaultButton(QWidget* view)
{
    QPushButton* b;
    foreach (auto w, view->children()) {
        if ((b = dynamic_cast<QPushButton*>(w)) != nullptr) {
            b->setAutoDefault(false);
            b->setDefault(false);
        }
    }
}

inline void VSCMainWindow::enableButtonState(QList<QPushButton*> buttons)
{
    foreach (auto button, buttons) {
        button->setEnabled(true);
    }
}

inline void VSCMainWindow::enableButtonState(QPushButton* button)
{
    button->setEnabled(true);
}

inline void VSCMainWindow::disableButtonState(QList<QPushButton*> buttons)
{
    foreach (auto button, buttons) {
        button->setEnabled(false);
    }
}

inline void VSCMainWindow::disableButtonState(QPushButton* button)
{
    button->setEnabled(false);
}

inline void VSCMainWindow::enableDefaultButton(QPushButton* button)
{
    button->setAutoDefault(true);
    button->setDefault(true);
}

inline void VSCMainWindow::disableDefaultButton(QPushButton* button)
{
    button->setAutoDefault(false);
    button->setDefault(false);
}

void VSCMainWindow::on_btn01SPCreate_clicked()
{
    resetDefaultButton(ui->pnlButtons);
    enableDefaultButton(ui->btn01SPCreate);
    ui->stackedWidget->setCurrentWidget(ui->pg01SPCreate);
}

void VSCMainWindow::on_btn02SPRemove_clicked()
{
    resetDefaultButton(ui->pnlButtons);
    enableDefaultButton(ui->btn02SPRemove);
    ui->stackedWidget->setCurrentWidget(ui->pg02SPRemove);
    ui->pg02SPRemove->setModel(m_vsp->portList());
    onActionPortList();
}

void VSCMainWindow::on_btn03LKCreate_clicked()
{
    resetDefaultButton(ui->pnlButtons);
    enableDefaultButton(ui->btn03LKCreate);
    ui->stackedWidget->setCurrentWidget(ui->pg03LKCreate);
    ui->pg03LKCreate->setModel(m_vsp->portList());
    onActionPortList();
}

void VSCMainWindow::on_btn04LKRemove_clicked()
{
    resetDefaultButton(ui->pnlButtons);
    enableDefaultButton(ui->btn04LKRemove);
    ui->stackedWidget->setCurrentWidget(ui->pg04LKRemove);
    ui->pg04LKRemove->setModel(m_vsp->linkList());
    onActionLinkList();
}

void VSCMainWindow::on_btn05PortList_clicked()
{
    resetDefaultButton(ui->pnlButtons);
    enableDefaultButton(ui->btn05PortList);
    ui->stackedWidget->setCurrentWidget(ui->pg05PortList);
    ui->pg05PortList->setModel(m_vsp->portList());
    onActionPortList();
}

void VSCMainWindow::on_btn06LinkList_clicked()
{
    resetDefaultButton(ui->pnlButtons);
    enableDefaultButton(ui->btn06LinkList);
    ui->stackedWidget->setCurrentWidget(ui->pg06LinkList);
    ui->pg06LinkList->setModel(m_vsp->linkList());
    onActionLinkList();
}

void VSCMainWindow::on_btn07Checks_clicked()
{
    resetDefaultButton(ui->pnlButtons);
    enableDefaultButton(ui->btn07Checks);
    ui->stackedWidget->setCurrentWidget(ui->pg07Checks);
    ui->pg07Checks->setModel(m_vsp->portList());
    onActionPortList();
}

void VSCMainWindow::on_btn08Traces_clicked()
{
    resetDefaultButton(ui->pnlButtons);
    enableDefaultButton(ui->btn08Traces);
    ui->stackedWidget->setCurrentWidget(ui->pg08Traces);
    ui->pg08Traces->setModel(m_vsp->portList());
    onActionPortList();
}

void VSCMainWindow::on_btn09Connect_clicked()
{
    resetDefaultButton(ui->pnlButtons);
    enableDefaultButton(ui->btn09Connect);
    ui->stackedWidget->setCurrentWidget(ui->pg09Connect);
}

void VSCMainWindow::on_btn10Close_clicked()
{
    resetDefaultButton(ui->pnlButtons);
    enableDefaultButton(ui->btn10Close);
}

void VSCMainWindow::onActionSPCreate()
{
    if (ui->stackedWidget->currentWidget() != ui->pg01SPCreate) {
        return;
    }

    VSPClient::TVSPPortParameters params;
    params.baudRate = ui->pg01SPCreate->baudRate();
    params.dataBits = ui->pg01SPCreate->dataBits();
    params.stopBits = ui->pg01SPCreate->stopBits();
    params.parity = ui->pg01SPCreate->parity();
    params.flowCtrl = ui->pg01SPCreate->flowCtrl();

    showOverlay();

    if (!m_vsp->CreatePort(&params)) {
        onClientError(-1, "CreatePort failed.");
        return;
    }
}

void VSCMainWindow::onActionSPRemove()
{
    if (ui->stackedWidget->currentWidget() == ui->pg02SPRemove) {
        showOverlay();
        if (!m_vsp->RemovePort(ui->pg02SPRemove->selection().id)) {
            onClientError(-1, "RemovePort failed.");
            return;
        }
    }
}

void VSCMainWindow::onActionLKCreate()
{
    if (ui->stackedWidget->currentWidget() == ui->pg03LKCreate) {
        VSPDataModel::TPortItem p1 = ui->pg03LKCreate->selection1();
        VSPDataModel::TPortItem p2 = ui->pg03LKCreate->selection2();
        showOverlay();
        if (!m_vsp->LinkPorts(p1.id, p2.id)) {
            onClientError(-1, "LinkPorts failed.");
            return;
        }
    }
}

void VSCMainWindow::onActionLKRemove()
{
    if (ui->stackedWidget->currentWidget() == ui->pg04LKRemove) {
        if (sender() != ui->btn04LKRemove) {
            VSPDataModel::TPortLink link = ui->pg04LKRemove->selection();
            showOverlay();
            if (!m_vsp->UnlinkPorts(link.source.id, link.target.id)) {
                onClientError(-1, "UnlinkPorts failed.");
                return;
            }
        }
        else {
            onActionLinkList();
        }
    }
}

void VSCMainWindow::onActionPortList()
{
    showOverlay();
    if (!m_vsp->GetPortList()) {
        onClientError(-1, "GetPortList failed.");
        return;
    }
}

void VSCMainWindow::onActionLinkList()
{
    showOverlay();
    if (!m_vsp->GetLinkList()) {
        onClientError(-1, "GetLinkList failed.");
        return;
    }
}

void VSCMainWindow::onActionEditChecks()
{
    showOverlay();
    if (!m_vsp->EnableChecks(1)) {
        onClientError(-1, "EnableChecks failed.");
        return;
    }
}

void VSCMainWindow::onActionEditTraces()
{
    showOverlay();
    if (!m_vsp->EnableTrace(1)) {
        onClientError(-1, "EnableTrace failed.");
        return;
    }
}

void VSCMainWindow::onActionVspConnect()
{
    showOverlay();
    if (!m_vsp->IsConnected() && !m_vsp->ConnectDriver()) {
        onClientError(-1, "ConnectDriver failed.");
        onClientDisconnected();

        if (sender() == ui->pg09Connect->button()) {
            // QDesktopServices::openUrl(QUrl(QStringLiteral("vspinstall://")));
            m_vsp->activateDriver();
        }

        return;
    }
    if (!m_vsp->GetStatus()) {
        ui->textBrowser->setPlainText("GetStatus failed.");
        removeOverlay();
        return;
    }
}
