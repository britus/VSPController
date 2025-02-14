#include "pgtrace.h"
#include "ui_pgtrace.h"

PGTrace::PGTrace(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PGTrace)
{
    ui->setupUi(this);
}

PGTrace::~PGTrace()
{
    delete ui;
}

QPushButton* PGTrace::button()
{
    return ui->btnUpdate;
}

void PGTrace::setModel(VSPDataModel* model)
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
