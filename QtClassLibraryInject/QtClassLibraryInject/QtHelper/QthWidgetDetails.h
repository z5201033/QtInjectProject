#pragma once

#include <QMap>
#include <QPointer>
#include <QDialog>

class QListWidget;
class QListWidgetItem;
class QTabWidget;

namespace Qth
{
	//////////////////////////////////////////////////////////////////////////
	// WidgetDetails
	class WidgetDetails : public QDialog
	{
		Q_OBJECT
	public:
		explicit WidgetDetails(QWidget* parent = nullptr);
		~WidgetDetails();
		void setTargetWidget(QWidget* widget);

	signals:
		void sigNeedUpdateWidget(QWidget* widget);

	private:
		void initUI();
		QWidget* addBaseInfoWidget();
		QWidget* addAttributeInfoWidget();
		QWidget* addStyleSheetWidget();

		void applyStyleSheet(bool sendSignal = false, bool showErrorMsg = false);
		void updateStyleSheet(bool showErrorMsg = false);

		void applyAttribute(bool sendSignal = false, bool showErrorMsg = false);
		void updateAttribute(bool showErrorMsg = false);

		void applyAllWidgetInfo();
		void updateAllWidgetInfo();

	private:
		typedef QMap<Qt::WidgetAttribute, QListWidgetItem*> AttributeItemMap;
		QPointer<QWidget>	m_targetWidget;
		QTabWidget*			m_tabWidget = nullptr;
		QListWidget*		m_attributeListWidget = nullptr;
		AttributeItemMap	m_attributeItemMap;
	};
}