#pragma once
#include "vspdatamodel.h"
#include <QObject>
#include <VSPController.hpp>

#define kIOErrorNotFound -536870160

using namespace VSPClient;

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

#define MAGIC_CONTROL    0xBE6605250000L
#define MAX_SERIAL_PORTS 16
#define MAX_PORT_LINKS   8

typedef struct {
    /* In whitch context calld */
    uint8_t context;

    /* User client command */
    uint8_t command;

    /* Command status response */
    struct Status {
        uint32_t code;
        uint64_t flags;
    } status;

    /* Command parameters */
    struct Parameter {
        /* parameter flags */
        uint64_t flags;

        /* port link parameters */
        struct PortLink {
            uint8_t source;
            uint8_t target;
        } link;
    } parameter;

    /* Available serial ports */
    struct PortList {
        uint8_t count;
        uint8_t list[MAX_SERIAL_PORTS];
    } ports;

    /* Available serial port links */
    struct LinkList {
        uint8_t count;
        uint64_t list[MAX_PORT_LINKS];
    } links;

} TVSPControllerData;

class VSPDriverClient: public QObject, public VSPController
{
    Q_OBJECT

public:
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

#define MAGIC_CONTROL    0xBE6605250000L
#define MAX_SERIAL_PORTS 16
#define MAX_PORT_LINKS   8

    typedef struct {
        /* In whitch context calld */
        uint8_t context;

        /* User client command */
        uint8_t command;

        /* Command status response */
        struct Status {
            uint32_t code;
            uint64_t flags;
        } status;

        /* Command parameters */
        struct Parameter {
            /* parameter flags */
            uint64_t flags;

            /* port link parameters */
            struct PortLink {
                uint8_t source;
                uint8_t target;
            } link;
        } parameter;

        /* Available serial ports */
        struct PortList {
            uint8_t count;
            uint8_t list[MAX_SERIAL_PORTS];
        } ports;

        /* Available serial port links */
        struct LinkList {
            uint8_t count;
            uint64_t list[MAX_PORT_LINKS];
        } links;

    } TVSPControllerData;

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
    void OnIOUCCallback(int result, void* args, uint32_t numArgs) override;
    void OnErrorOccured(int error, const char* message) override;
    void OnDataReady(void* data) override;

private:
    VSPPortListModel m_portList;
    VSPLinkListModel m_linkList;
};
