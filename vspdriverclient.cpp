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

void VSPDriverClient::OnIOUCCallback(int result, void* args, uint32_t size)
{
    const TVSPControllerData* data = (TVSPControllerData*) (args);

    QByteArray buffer;
    QTextStream text(&buffer);

    // make sure we have an error code
    result =
       (result == 0 //
           ? data->status.code
           : result);

    QByteArray txStatus =
       (data->context != 4 //
           ? " success"
           : " failed #" + QString::number(result & 0x00000000ffffffff, 16).toUtf8());

    text << "Driver callback result:" << Qt::endl;
    text << "Data size......: " << size << Qt::endl;
    text << "Context........: " << data->context << Qt::endl;
    text << "Command........: " << data->command << txStatus << Qt::endl;
    text << "Status code....: " << data->status.code << Qt::endl;
    text << "Status flags...: " << Qt::hex << data->status.flags << Qt::endl;
    text << "Parameter flags: " << Qt::hex << data->parameter.flags << Qt::endl;
    text << "Port 1.........: " << data->parameter.link.source << Qt::endl;
    text << "Port 2.........: " << data->parameter.link.target << Qt::endl;
    text << "Port count.....: " << data->ports.count << Qt::endl;
    text << "Link count.....: " << data->links.count << Qt::endl;

    if (data->ports.count) {
        m_portList.resetModel();
        for (uint i = 0; i < data->ports.count; i++) {
            const uint8_t pid = data->ports.list[i];
            text << "Port item......: " << pid << Qt::endl;
            m_portList.append(VSPDataModel::TPortItem({pid, tr("Port %1").arg(pid)}));
            continue;
        }
    }
    else if (data->command == vspControlRemovePort) {
        m_portList.resetModel();
    }

    if (data->links.count) {
        m_linkList.resetModel();
        for (uint i = 0; i < data->links.count; i++) {
            const uint8_t lid = (data->links.list[i] >> 16) & 0x000000ff;
            const uint8_t src = (data->links.list[i] >> 8) & 0x000000ff;
            const uint8_t tgt = (data->links.list[i]) & 0x000000ff;
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
    }
    else if (data->command == vspControlUnlinkPorts) {
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
