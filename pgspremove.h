#pragma once

#include "vspdatamodel.h"
#include <QPushButton>
#include <QWidget>

namespace Ui {
class PGSPRemove;
}

class PGSPRemove: public QWidget
{
    Q_OBJECT

public:
    explicit PGSPRemove(QWidget* parent = nullptr);
    ~PGSPRemove();
    QPushButton* button();
    void setModel(const VSPDataModel* model);
    VSPDataModel::TPortItem selection() const;

private:
    Ui::PGSPRemove* ui;
};
