#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsAppDemo.h"

class QtWidgetsAppDemo : public QMainWindow
{
    Q_OBJECT

public:
    QtWidgetsAppDemo(QWidget *parent = nullptr);
    ~QtWidgetsAppDemo();

private:
    Ui::QtWidgetsAppDemoClass ui;
};
