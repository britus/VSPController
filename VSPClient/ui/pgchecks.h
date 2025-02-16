#pragma once

#include "vspdatamodel.h"
#include <QPushButton>
#include <QWidget>

namespace Ui {
class PGChecks;
}

class PGChecks: public QWidget
{
    Q_OBJECT

public:
    explicit PGChecks(QWidget* parent = nullptr);
    ~PGChecks();
    QPushButton* button();
    void setModel(VSPDataModel* model);

private:
    Ui::PGChecks* ui;
};
