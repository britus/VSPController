#include "pgchecks.h"
#include "ui_pgchecks.h"

PGChecks::PGChecks(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PGChecks)
{
    ui->setupUi(this);
}

PGChecks::~PGChecks()
{
    delete ui;
}

QPushButton* PGChecks::button()
{
    return ui->btnUpdate;
}

void PGChecks::setModel(VSPDataModel* /*model*/)
{
#if 0
    if (!property("isConnected").toBool()) {
        setProperty("isConnected", true);
        connect(model, &QAbstractTableModel::modelReset, this, [this, model]() {
            ui->cbPorts->clear();
            for (int i = 0; i < model->rowCount(); i++) {
                VSPDataModel::TDataRecord r = model->at(i).value<VSPDataModel::TDataRecord>();
                ui->cbPorts->addItem(r.port.name, QVariant::fromValue(r.port));
            }
        });
    }
#endif
}
