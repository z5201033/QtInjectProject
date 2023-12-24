#pragma once

#include <QListWidget>

namespace Qth
{
	//////////////////////////////////////////////////////////////////////////
	//AttributeListItem
	class AttributeListItem : public QListWidgetItem
	{
	public:
		const static int s_arributeRole = Qt::UserRole;

		explicit AttributeListItem(Qt::WidgetAttribute attri, QListWidget* listview = nullptr);
		bool operator<(const QListWidgetItem& other) const override;
	};

	//////////////////////////////////////////////////////////////////////////
	//WindowsFlagListItem
	class WindowsFlagListItem : public QListWidgetItem
	{
	public:
		const static int s_windowsFlagRole = Qt::UserRole;

		explicit WindowsFlagListItem(Qt::WindowType type, QListWidget* listview = nullptr);
		bool operator<(const QListWidgetItem& other) const override;
	};
}