#pragma once

#include "vspdatamodel.h"
#include <QPushButton>
#include <QWidget>

namespace Ui {
class PGLinkList;
}

class PGLinkList: public QWidget
{
    Q_OBJECT

public:
    explicit PGLinkList(QWidget* parent = nullptr);
    ~PGLinkList();
    QPushButton* button();
    void setModel(VSPDataModel* model);

private:
    Ui::PGLinkList* ui;
};
