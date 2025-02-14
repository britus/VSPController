#pragma once

#include "ui_pglkcreate.h"
#include "vspdatamodel.h"
#include <QPushButton>
#include <QWidget>

namespace Ui {
class PGLKCreate;
}

class PGLKCreate: public QWidget
{
    Q_OBJECT

public:
    explicit PGLKCreate(QWidget* parent = nullptr);
    ~PGLKCreate();
    QPushButton* button();
    void setModel(const VSPDataModel* model);
    VSPDataModel::TPortItem selection1() const;
    VSPDataModel::TPortItem selection2() const;

private:
    Ui::PGLKCreate* ui;
};
