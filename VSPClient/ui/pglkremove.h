#pragma once

#include "vspdatamodel.h"
#include <QPushButton>
#include <QWidget>

namespace Ui {
class PGLKRemove;
}

class PGLKRemove: public QWidget
{
    Q_OBJECT

public:
    explicit PGLKRemove(QWidget* parent = nullptr);
    ~PGLKRemove();
    QPushButton* button();
    void setModel(const VSPDataModel* model);
    VSPDataModel::TPortLink selection() const;

private:
    Ui::PGLKRemove* ui;
};
