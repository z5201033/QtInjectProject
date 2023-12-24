#pragma once

#include <QPointer>
#include <QDialog>

namespace Qth
{
	class WatchObjectWidget : public QDialog
	{
		Q_OBJECT
	public:
		WatchObjectWidget(QWidget* parent);
		~WatchObjectWidget();
		void setFilterObject(QObject* obj);

	signals:
		void sigHighlight(QObject* obj);
	protected:
		bool eventFilter(QObject* watched, QEvent* event) override;
	private slots:
		void onPrint();
		void onShowFilter();
		void onClear();
		void onHighlight();
	private:
		void initData();
		void initUI();
		void initConnect();
		void startWatch();
		void stopWatch();
		bool needWatch(QObject* watched, QEvent* event);
	private:
		struct WatchObjectWidgetPrivate* d;
		QPointer<QObject> _filterObj;
	};
}