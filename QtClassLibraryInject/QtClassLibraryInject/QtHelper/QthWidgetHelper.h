#pragma once

#include <QWidget>

namespace Qth
{
	class WidgetHelper : public QWidget
	{
		Q_OBJECT
	public:
		explicit WidgetHelper(QWidget* parent = nullptr);
		~WidgetHelper();

	private:
	};
}