#include "pglkremove.h"
#include "ui_pglkremove.h"

PGLKRemove::PGLKRemove(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PGLKRemove)
{
    ui->setupUi(this);
}

PGLKRemove::~PGLKRemove()
{
    delete ui;
}

QPushButton* PGLKRemove::button()
{
    return ui->btnUnlink;
};

void PGLKRemove::setModel(const VSPDataModel* model)
{
    if (!property("isConnected").toBool()) {
        setProperty("isConnected", true);
        connect(model, &QAbstractTableModel::modelReset, this, [this, model]() {
            ui->comboBox->clear();

            for (int i = 0; i < model->rowCount(); i++) {
                VSPDataModel::TDataRecord r = model->at(i).value<VSPDataModel::TDataRecord>();
                ui->comboBox->addItem(r.link.name, QVariant::fromValue(r.link));
            }

            ui->comboBox->setEnabled(ui->comboBox->count() > 0);
            ui->btnUnlink->setEnabled(ui->comboBox->count() > 0);
        });
    }
}

VSPDataModel::TPortLink PGLKRemove::selection() const
{
    const QVariant v = ui->comboBox->currentData();
    if (v.isNull() || !v.isValid()) {
        return {};
    }
    return v.value<VSPDataModel::TPortLink>();
}
