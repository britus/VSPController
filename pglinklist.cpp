#include "pglinklist.h"
#include "ui_pglinklist.h"
#include <QTimer>

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

    QTimer::singleShot(50, this, [this, model]() {
        for (int i = 0; i < model->columnCount(); i++) {
            switch (i) {
                case 0: {
                    ui->tableView->setColumnWidth(i, 40);
                    break;
                }
                case 1: {
                    ui->tableView->setColumnWidth(i, 260);
                    break;
                }
                case 2: {
                    ui->tableView->setColumnWidth(i, 80);
                    break;
                }
                case 3: {
                    ui->tableView->setColumnWidth(i, 80);
                    break;
                }
            }
        }
    });
}
