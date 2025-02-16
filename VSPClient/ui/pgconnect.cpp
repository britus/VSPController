#include "pgconnect.h"
#include "ui_pgconnect.h"

PGConnect::PGConnect(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PGConnect)
{
    ui->setupUi(this);
}

PGConnect::~PGConnect()
{
    delete ui;
}

QPushButton* PGConnect::button()
{
    return ui->btnConnect;
};
