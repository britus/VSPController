// ********************************************************************
// vscmainwindow.h - Application window
//
// Copyright © 2025 by EoF Software Labs
// Copyright © 2024 Apple Inc. (some copied parts)
// SPDX-License-Identifier: MIT
// ********************************************************************
#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <vspabstractpage.h>
#include <vspdatamodel.h>
#include <vspdriverclient.h>

QT_BEGIN_NAMESPACE

namespace Ui {
class VSCMainWindow;
}

QT_END_NAMESPACE

class VSCMainWindow: public QMainWindow
{
    Q_OBJECT

public:
    VSCMainWindow(QWidget* parent = nullptr);
    ~VSCMainWindow();

private slots:
    void onClientConnected();
    void onClientDisconnected();
    void onClientError(int error, const QString& message);
    void onUpdateStatusLog(const QByteArray& message);
    void onUpdateButtons(bool enabled = false);
    void onCommandResult(TVSPControlCommand command, VSPPortListModel* portModel, VSPLinkListModel* linkModel);
    void onComplete();

    void onSetupFailWithError(uint32_t code, const char* message);
    void onSetupFinishWithResult(uint32_t code, const char* message);
    void onSetupNeedsUserApproval();

    void onSelectPage();
    void onActionExecute(const TVSPControlCommand command, const QVariant& data);

protected:
    void resizeEvent(QResizeEvent* event) override;

protected slots:
    virtual void updateOverlayGeometry();

private:
    Ui::VSCMainWindow* ui;
    VSPDriverClient* m_vsp;
    QMap<QPushButton*, VSPAbstractPage*> m_buttonMap;

private:
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
