#include "QthCustomListItem.h"

namespace Qth
{
	//////////////////////////////////////////////////////////////////////////
	//AttributeListItem
	AttributeListItem::AttributeListItem(Qt::WidgetAttribute attri, QListWidget* listview/* = nullptr*/)
		: QListWidgetItem(listview)
	{
		setData(s_arributeRole, (int)attri);
	}

	bool AttributeListItem::operator<(const QListWidgetItem& other) const
	{
		bool curCheck = checkState() == Qt::Checked;
		bool otherCheck = other.checkState() == Qt::Checked;
		if (curCheck != otherCheck)
			return curCheck;

		int curAttri = data(s_arributeRole).toInt();
		int otherAttri = other.data(s_arributeRole).toInt();

		return curAttri < otherAttri;
	}


	//////////////////////////////////////////////////////////////////////////
	//WindowsFlagListItem
	WindowsFlagListItem::WindowsFlagListItem(Qt::WindowType type, QListWidget* listview/* = nullptr*/)
	{
		setData(s_windowsFlagRole, (unsigned int)type);
	}

	bool WindowsFlagListItem::operator<(const QListWidgetItem& other) const
	{
		bool curCheck = checkState() == Qt::Checked;
		bool otherCheck = other.checkState() == Qt::Checked;
		if (curCheck != otherCheck)
			return curCheck;

		unsigned int curAttri = data(s_windowsFlagRole).toUInt();
		unsigned int otherAttri = other.data(s_windowsFlagRole).toUInt();

		return curAttri < otherAttri;
	}
}