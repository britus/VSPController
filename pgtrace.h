#pragma once

#include "vspdatamodel.h"
#include <QPushButton>
#include <QWidget>

namespace Ui {
class PGTrace;
}

class PGTrace: public QWidget
{
    Q_OBJECT

public:
    explicit PGTrace(QWidget* parent = nullptr);
    ~PGTrace();
    QPushButton* button();
    void setModel(VSPDataModel* model);

private:
    Ui::PGTrace* ui;
};
