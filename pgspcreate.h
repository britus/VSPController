#pragma once

#include "vspdatamodel.h"
#include <QPushButton>
#include <QWidget>

namespace Ui {
class PGSPCreate;
}

class PGSPCreate: public QWidget
{
    Q_OBJECT

public:
    explicit PGSPCreate(QWidget* parent = nullptr);
    ~PGSPCreate();
    QPushButton* button();
    void setModel(VSPDataModel* model);

private:
    Ui::PGSPCreate* ui;
};
