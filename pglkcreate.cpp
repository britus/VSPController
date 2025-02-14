#include "pglkcreate.h"
#include "ui_pglkcreate.h"

PGLKCreate::PGLKCreate(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PGLKCreate)
{
    ui->setupUi(this);
}

PGLKCreate::~PGLKCreate()
{
    delete ui;
}

QPushButton* PGLKCreate::button()
{
    return ui->btnLinkPorts;
}

void PGLKCreate::setModel(const VSPDataModel* model)
{
    if (!property("isConnected").toBool()) {
        setProperty("isConnected", true);
        connect(model, &QAbstractTableModel::modelReset, this, [this, model]() {
            ui->cbPort1->clear();
            ui->cbPort2->clear();
            for (int i = 0; i < model->rowCount(); i++) {
                VSPDataModel::TDataRecord r = model->at(i).value<VSPDataModel::TDataRecord>();
                ui->cbPort1->addItem(r.port.name, QVariant::fromValue(r.port));
                ui->cbPort2->addItem(r.port.name, QVariant::fromValue(r.port));
            }
        });
    }
}

VSPDataModel::TPortItem PGLKCreate::selection1() const
{
    const QVariant v = ui->cbPort1->currentData();
    if (v.isNull() || !v.isValid()) {
        return {};
    }
    return v.value<VSPDataModel::TPortItem>();
}

VSPDataModel::TPortItem PGLKCreate::selection2() const
{
    const QVariant v = ui->cbPort2->currentData();
    if (v.isNull() || !v.isValid()) {
        return {};
    }
    return v.value<VSPDataModel::TPortItem>();
}
