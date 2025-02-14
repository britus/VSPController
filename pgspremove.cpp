#include "pgspremove.h"
#include "ui_pgspremove.h"

PGSPRemove::PGSPRemove(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PGSPRemove)
{
    ui->setupUi(this);
}

PGSPRemove::~PGSPRemove()
{
    delete ui;
}

QPushButton* PGSPRemove::button()
{
    return ui->btnDoSPRemove;
}

void PGSPRemove::setModel(const VSPDataModel* model)
{
    if (!property("isConnected").toBool()) {
        setProperty("isConnected", true);
        connect(model, &QAbstractTableModel::modelReset, this, [this, model]() {
            ui->cbSerialPorts->clear();
            for (int i = 0; i < model->rowCount(); i++) {
                VSPDataModel::TDataRecord r = model->at(i).value<VSPDataModel::TDataRecord>();
                ui->cbSerialPorts->addItem(r.port.name, QVariant::fromValue(r.port));
            }
        });
    }
}

VSPDataModel::TPortItem PGSPRemove::selection() const
{
    const QVariant v = ui->cbSerialPorts->currentData();
    if (v.isNull() || !v.isValid()) {
        return {};
    }
    return v.value<VSPDataModel::TPortItem>();
}
