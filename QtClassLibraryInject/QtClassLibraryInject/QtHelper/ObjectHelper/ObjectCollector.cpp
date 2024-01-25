#include "ObjectCollector.h"

#include <QApplication>
#include <QGuiApplication>
#include <QHash>
#include <QPointer>
#include <QWidget>
#include <QWindow>

namespace Qth
{
	struct ObjectCollectorPrivate
	{
		typedef QHash<QObject*, QPointer<QObject>> ObjectsCache;
		ObjectsCache		_objectsCache;
		bool				_first = true;
	};

	ObjectCollector::ObjectCollector(QObject* parent)
		: QObject(parent)
		, d(new ObjectCollectorPrivate)
	{
	}

	ObjectCollector::~ObjectCollector()
	{
	}

	void ObjectCollector::init()
	{
		if (qApp)
			qApp->installEventFilter(this);
	}

	void ObjectCollector::release()
	{
		if (qApp)
			qApp->removeEventFilter(this);
	}

	bool ObjectCollector::eventFilter(QObject* watched, QEvent* event)
	{
		d->_objectsCache[watched] = watched;
		return QObject::eventFilter(watched, event);
	}

	QSet<QObject*> ObjectCollector::topObjects()
	{
		if (d->_first)
		{
			d->_first = false;
			addTopObjectToCache();
		}

		QSet<QObject*> objects;
		for (auto it = d->_objectsCache.begin(); it != d->_objectsCache.end();)
		{
			QObject* cur = it.value();
			if (!cur) {
				it = d->_objectsCache.erase(it);
				continue;
			}

			while (cur->parent()) {
				cur = cur->parent();
			}
			objects.insert(cur);
			++it;
		}

		return objects;
	}

	void ObjectCollector::addTopObjectToCache()
	{
		for (QWidget* w : QApplication::topLevelWidgets())
		{
			d->_objectsCache[w] = w;
		}

		for (QWindow* w : QGuiApplication::topLevelWindows())
		{
			d->_objectsCache[w] = w;
		}
	}
}