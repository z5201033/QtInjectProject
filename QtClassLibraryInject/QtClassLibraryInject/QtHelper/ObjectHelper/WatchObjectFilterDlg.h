#pragma once

#include <QPointer>
#include <QDialog>
#include <QEvent>

class QListWidgetItem;
namespace Qth
{
	class WatchObjectFilterDlg : public QDialog
	{
		Q_OBJECT
	public:
		WatchObjectFilterDlg(QWidget* parent);
		~WatchObjectFilterDlg();
		const QMap<QEvent::Type, bool>& filterItem();
		void resetToDefault();
	private slots:
		void onCheckAll();
		void onCheckSelected();
		void onReset();
		void onItemChanged(QListWidgetItem* item);
	private:
		void initData();
		void initUI();
		void initConnect();
		QWidget* initTypeList();
	private:
		struct WatchObjectFilterDlgPrivate* d;
	};
}