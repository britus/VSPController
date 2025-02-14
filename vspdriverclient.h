#pragma once
#include "vspdatamodel.h"
#include <QObject>
#include <VSPController.hpp>

#define kIOErrorNotFound -536870160

typedef enum {
    vspContextPing = 0x01,
    vspContextPort = 0x02,
    vspContextResult = 0x03,
    vspContextError = 0x04,
} TVSPUserContext;

typedef enum {
    vspControlPingPong,
    vspControlGetStatus,
    vspControlCreatePort,
    vspControlRemovePort,
    vspControlLinkPorts,
    vspControlUnlinkPorts,
    vspControlGetPortList,
    vspControlGetLinkList,
    vspControlEnableChecks,
    vspControlEnableTrace,
    // Has to be last
    vspLastCommand,
} TVSPControlCommand;

using namespace VSPClient;

class VSPDriverClient: public QObject, public VSPController
{
    Q_OBJECT

public:
    VSPDriverClient(QObject* parent = nullptr);

    inline VSPPortListModel* portList()
    {
        return &m_portList;
    }

    inline VSPLinkListModel* linkList()
    {
        return &m_linkList;
    }

signals:
    void connected();
    void disconnected();
    void errorOccured(int error, const QString& message);
    void updateStatusLog(const QByteArray& message);
    void updateButtons(bool enabled = false);
    void complete();

protected:
    void OnConnected() override;
    void OnDisconnected() override;
    void OnIOUCCallback(int result, void** args, uint32_t numArgs) override;
    void OnErrorOccured(int error, const char* message) override;
    void OnDataReady(void* data) override;

private:
    VSPPortListModel m_portList;
    VSPLinkListModel m_linkList;
};
