#pragma once

#include <QPointer>
#include <QDialog>

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

	private slots:
		void onTargetVisibelStateChanged(int state);
		void onApplyCurrentTab();
		void onApplyAllWidgetInfo();
		void onUpdateAllWidgetInfo();
	private:
		void initUI();
		QWidget* addBaseInfoWidget();
		QWidget* addAttributeInfoWidget();
		QWidget* addWindowFlagWidget();
		QWidget* addStyleSheetWidget();

		void applyBaseInfo(bool sendSignal = false, bool showErrorMsg = false);
		void updateBaseInfo(bool showErrorMsg = false);

		void applyAttribute(bool sendSignal = false, bool showErrorMsg = false);
		void updateAttribute(bool showErrorMsg = false);

		void applyWindowFlag(bool sendSignal = false, bool showErrorMsg = false);
		void updateWindowFlag(bool showErrorMsg = false);

		void applyStyleSheet(bool sendSignal = false, bool showErrorMsg = false);
		void updateStyleSheet(bool showErrorMsg = false);

		bool checkTargetWidgetValid(bool showErrorMsg = false);
	private:
		QPointer<QWidget>	m_targetWidget;

		struct WidgetDetailsrivate* d;
	};
}