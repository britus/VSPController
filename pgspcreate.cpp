#include "pgspcreate.h"
#include "ui_pgspcreate.h"

PGSPCreate::PGSPCreate(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PGSPCreate)
{
    ui->setupUi(this);
}

PGSPCreate::~PGSPCreate()
{
    delete ui;
}

QPushButton* PGSPCreate::button()
{
    return ui->btnDoSPCreate;
}

void PGSPCreate::setModel(VSPDataModel* model)
{
}
