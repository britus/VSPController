#pragma once

#include <QPushButton>
#include <QWidget>

namespace Ui {
class PGConnect;
}

class PGConnect: public QWidget
{
    Q_OBJECT

public:
    explicit PGConnect(QWidget* parent = nullptr);
    ~PGConnect();
    QPushButton* button();

private:
    Ui::PGConnect* ui;
};
