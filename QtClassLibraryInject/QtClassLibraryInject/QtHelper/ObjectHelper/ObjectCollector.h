#pragma once

#include <QObject>
#include <QSet>

namespace Qth
{
	class ObjectCollector : public QObject
	{
		Q_OBJECT
	public:
		ObjectCollector(QObject* parent);
		~ObjectCollector();
		void init();
		void release();
		QSet<QObject*> topObjects();

	protected:
		bool eventFilter(QObject* watched, QEvent* event) override;

	private:
		void addTopWidgetToCache();

	private:
		struct ObjectCollectorPrivate* d;
	};
}