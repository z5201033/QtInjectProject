#pragma once

#include <QWidget>

extern "C" typedef QWidget* (*generateQtHelperNetWorkWidgetFunc)(QWidget* parent);
extern "C" QWidget* generateQtHelperNetWorkWidget(QWidget * parent);