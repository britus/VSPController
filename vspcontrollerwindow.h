#pragma once
#include "vspdatamodel.h"
#include "vspdriverclient.h"
#include <QMainWindow>
#include <QPushButton>

QT_BEGIN_NAMESPACE

namespace Ui {
class VSPControllerWindow;
}

QT_END_NAMESPACE

class VSPControllerWindow: public QMainWindow
{
    Q_OBJECT

public:
    VSPControllerWindow(QWidget* parent = nullptr);
    ~VSPControllerWindow();

private slots:
    void onClientConnected();
    void onClientDisconnected();
    void onClientError(int error, const QString& message);
    void onUpdateStatusLog(const QByteArray& message);
    void onUpdateButtons(bool enabled = false);
    void onComplete();
    void onActionSPCreate();
    void onActionSPRemove();
    void onActionLKCreate();
    void onActionLKRemove();
    void onActionPortList();
    void onActionLinkList();
    void onActionEditChecks();
    void onActionEditTraces();
    void onActionVspConnect();

    void on_btn01SPCreate_clicked();
    void on_btn02SPRemove_clicked();
    void on_btn03LKCreate_clicked();
    void on_btn04LKRemove_clicked();
    void on_btn05PortList_clicked();
    void on_btn06LinkList_clicked();
    void on_btn07Checks_clicked();
    void on_btn08Traces_clicked();
    void on_btn09Connect_clicked();
    void on_btn10Close_clicked();

protected:
    void resizeEvent(QResizeEvent* event) override;

protected slots:
    virtual void updateOverlayGeometry();

private:
    Ui::VSPControllerWindow* ui;
    VSPDriverClient* m_vsp;

    inline void resetDefaultButton(QWidget* view);
    inline void enableDefaultButton(QPushButton* button);
    inline void disableDefaultButton(QPushButton* button);
    inline void enableButtonState(QList<QPushButton*> buttons);
    inline void enableButtonState(QPushButton* button);
    inline void disableButtonState(QList<QPushButton*> buttons);
    inline void disableButtonState(QPushButton* button);
    inline void showOverlay();
    inline void removeOverlay();
};
