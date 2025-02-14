#include "pglinklist.h"
#include "ui_pglinklist.h"

PGLinkList::PGLinkList(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PGLinkList)
{
    ui->setupUi(this);
}

PGLinkList::~PGLinkList()
{
    delete ui;
}

QPushButton* PGLinkList::button()
{
    return ui->btnRefresh;
}

void PGLinkList::setModel(VSPDataModel* model)
{
    ui->tableView->setModel(model);
}
