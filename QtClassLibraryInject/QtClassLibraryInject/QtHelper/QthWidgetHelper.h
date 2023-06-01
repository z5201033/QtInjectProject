#pragma once

#include <QMap>
#include <QPointer>
#include <QWidget>

class QTreeWidget;
class QTreeWidgetItem;

namespace Qth
{
	class CaptureDlgMgr;
	class TreeInfoWidgetHelper;
	class WidgetDetails;

	//////////////////////////////////////////////////////////////////////////
	// WidgetHelper
	class WidgetHelper : public QWidget
	{
		Q_OBJECT
	public:
		explicit WidgetHelper(QWidget* parent = nullptr);
		~WidgetHelper();

	public:
		static QString getWidgetAttributeString(QWidget* curWidget);
		static QWidget* convertAddrToWidget(quint64 addr);

	protected slots:
		void onPickerBtnPressed();
		void onFindWidgetBtnPressed();
		void onRefreshBtnPressed();
		void onHighLightWidget(QWidget* widget);
		void onCatchWidgetChanged(QWidget* targetWidget);
		void onCatchWidgetFinish(QWidget* targetWidget);

	private:
		void initUI();
		QWidget* createTitle();
		QWidget* createreeInfoWidge();
		void updateCatchWidgetInfo(QWidget* targetWidget, bool force = false);

	private:
		CaptureDlgMgr*			m_captureDlgMgr = nullptr;
		QPointer<QWidget>		m_currentCaptureWidget;
		TreeInfoWidgetHelper*	m_treeInfoWidgetHelper = nullptr;
	};

	//////////////////////////////////////////////////////////////////////////
	// TreeInfoWidget
	class TreeInfoWidgetHelper : public QWidget
	{
		Q_OBJECT
	public:
		explicit TreeInfoWidgetHelper(QWidget* parent = nullptr);
		~TreeInfoWidgetHelper();

		void clearAllInfo();
		void setWidgetRootNode(QWidgetList rootList);
		void addWidgetRootNode(QWidget* root);
		void showAllWidgetInfo();
		bool findAndScrollToTargetInTree(QWidget* target);

	signals:
		void sigHighLightWidget(QWidget* widget);

	private:
		void initSignalConnect();
		void setItemInfo(QTreeWidgetItem* item, QWidget* node);
		void addChildInfo(QTreeWidgetItem* parentItem, QWidget* parentWidget, bool recursion = true);
		int itemDepth(QTreeWidgetItem* parentItem);
		void showMenu(QTreeWidgetItem* item, int column);
		void showWidgetInfoDetailed(QWidget* widget);
	private:
		typedef QHash<QTreeWidgetItem*, QWidget*> TreeItemToWidget;
		typedef QHash<QWidget*, QTreeWidgetItem*> WidgetToTreeItem;

		QTreeWidget*			m_treeInfo = nullptr;
		QStringList			m_headerList;
		QMap<QString, int>	m_headerToIndex;
		QTreeWidgetItem*		m_selectItemLast = nullptr;

		TreeItemToWidget		m_treeItemToWidget;
		WidgetToTreeItem		m_widgetToTreeItem;

		WidgetDetails*			m_widgetDetails = nullptr;
	};
}