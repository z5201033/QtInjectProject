#pragma once

#include <QString>
#include <QMetaEnum>

namespace Qth
{
	class QthEnumUtils
	{
	public:
		template<typename T>
		static QString getEnumName(T enValue, bool fullName = true);
		template<typename T>
		static QMetaEnum getMetaEnum(T enValue);
	};

	template<typename T>
	QString QthEnumUtils::getEnumName(T enValue, bool fullName) {
		static_assert(QtPrivate::IsQEnumHelper<T>::Value);

		QString ret;
		const QMetaObject* meta = qt_getEnumMetaObject(enValue);
		if (!meta)
			return ret;

		const char* name = qt_getEnumName(enValue);
		QMetaEnum me = meta->enumerator(meta->indexOfEnumerator(name));
		const char* key = me.valueToKey(enValue);
		if (!key)
			return ret;

		if (me.scope())
			ret += me.scope();
		else
			ret += me.enumName();
		ret += "::";
		ret += key;
		return ret;
	}

	template<typename T>
	QMetaEnum QthEnumUtils::getMetaEnum(T enValue) {
		static_assert(QtPrivate::IsQEnumHelper<T>::Value);
		const QMetaObject* obj = qt_getEnumMetaObject(enValue);
		if (!obj)
			return QMetaEnum();

		const char* name = qt_getEnumName(enValue);
		return obj->enumerator(obj->indexOfEnumerator(name));
	}
}