#include "vspdriverclient.h"
#include <QDebug>
#include <QTextStream>
#include <QTimer>

#define MAX_SERIAL_PORTS 16
#define MAX_PORT_LINKS   8

VSPDriverClient::VSPDriverClient(QObject* parent)
    : QObject(parent)
{
}

void VSPDriverClient::OnConnected()
{
    emit connected();
}

void VSPDriverClient::OnDisconnected()
{
    emit disconnected();
}

void VSPDriverClient::OnIOUCCallback(int result, void** args, uint32_t numArgs)
{
    struct vsp_header {
        uint64_t context;
        uint64_t command;
        uint64_t paramFlags;
        uint64_t portSource;
        uint64_t portTarget;
        uint64_t statusCode;
        uint64_t statusFlags;
        uint64_t listCount;
    };
    const struct vsp_header* header = (struct vsp_header*) (args);
    const int64_t* arrayArgs = (const int64_t*) args;

    QByteArray buffer;
    QTextStream text(&buffer);

    // make sure we have an error code
    result =
       (result == 0 //
           ? header->statusCode
           : result);

    QByteArray txStatus =
       (header->context != 4 //
           ? " success"
           : " failed #" + QString::number(result & 0x00000000ffffffff, 16).toUtf8());

    text << "Driver callback result:" << Qt::endl;
    text << "Command........: " << header->command << txStatus << Qt::endl;
    text << "Context........: " << header->context << Qt::endl;
    text << "Parameter flags: " << Qt::hex << header->paramFlags << Qt::endl;
    text << "Port 1.........: " << header->portSource << Qt::endl;
    text << "Port 2.........: " << header->portTarget << Qt::endl;
    text << "List count.....: " << header->listCount << Qt::endl;
    text << "Status code....: " << header->statusCode << Qt::endl;
    text << "Status flags...: " << Qt::hex << header->statusFlags << Qt::endl;

    if (header->listCount) {
        uint8_t offset = 8;
        switch (header->command) {
            case vspControlCreatePort:
            case vspControlRemovePort:
            case vspControlGetPortList:
            case vspControlGetStatus: {
                m_portList.resetModel();
                for (uint32_t i = 0; i < header->listCount && (offset + i) < numArgs; i++) {
                    const uint8_t pid = (*(arrayArgs + offset + i));
                    text << "Port item......: " << pid << Qt::endl;
                    m_portList.append(VSPDataModel::TPortItem({pid, tr("Port %1").arg(pid)}));
                    continue;
                }
                break;
            }
            case vspControlLinkPorts:
            case vspControlUnlinkPorts:
            case vspControlGetLinkList: {
                m_linkList.resetModel();
                for (uint32_t i = 0; i < header->listCount && (offset + i) < numArgs; i++) {
                    const uint8_t lid = ((*(arrayArgs + offset + i)) >> 16) & 0x000000ff;
                    const uint8_t src = ((*(arrayArgs + offset + i)) >> 8) & 0x000000ff;
                    const uint8_t tgt = ((*(arrayArgs + offset + i))) & 0x000000ff;
                    VSPDataModel::TPortItem p1 = {};
                    VSPDataModel::TPortItem p2 = {};
                    QString name = tr("[Port A: %1 <-> Port B: %2]").arg(src).arg(tgt);
                    text << "Link item......: " << lid << " " << name << Qt::endl;
                    for (int i = 0; i < m_portList.rowCount(); i++) {
                        VSPDataModel::TDataRecord r = m_portList.at(i).value<VSPDataModel::TDataRecord>();
                        if (r.port.id == src) {
                            p1 = r.port;
                        }
                        if (r.port.id == tgt) {
                            p2 = r.port;
                        }
                        if (p1.id && p2.id) {
                            break;
                        }
                    }
                    m_linkList.append(VSPDataModel::TPortLink(
                       {lid, //
                        tr("Port Link %1 %2").arg(lid).arg(name),
                        p1,
                        p2}));
                    continue;
                }
                break;
            }
        }
    }
    else {
        m_portList.resetModel();
        m_linkList.resetModel();
    }

    // Overlay-Größe anpassen
    QTimer* t = new QTimer(this);
    connect(t, &QTimer::timeout, this, [this, txStatus, buffer, result]() {
        emit updateStatusLog(buffer);
        emit updateButtons(true);
        if (result != 0) {
            emit errorOccured(result, txStatus);
        }
        emit complete();
    });
    t->setSingleShot(true);
    t->start(200);
}

void VSPDriverClient::OnErrorOccured(int error, const char* message)
{
    emit errorOccured(error, message);
}

void VSPDriverClient::OnDataReady(void* /*data internal VSP data struct*/)
{
}
