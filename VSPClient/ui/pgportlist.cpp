#include "pgportlist.h"
#include "ui_pgportlist.h"

PGPortList::PGPortList(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PGPortList)
{
    ui->setupUi(this);
}

PGPortList::~PGPortList()
{
    delete ui;
}

QPushButton* PGPortList::button()
{
    return ui->btnRefresh;
};

void PGPortList::setModel(VSPDataModel* model)
{
    ui->tableView->setModel(model);
}
