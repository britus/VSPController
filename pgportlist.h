#pragma once

#include "vspdatamodel.h"
#include <QPushButton>
#include <QWidget>

namespace Ui {
class PGPortList;
}

class PGPortList: public QWidget
{
    Q_OBJECT

public:
    explicit PGPortList(QWidget* parent = nullptr);
    ~PGPortList();
    QPushButton* button();
    void setModel(VSPDataModel* model);

private:
    Ui::PGPortList* ui;
};
