#pragma once

#include <QMap>
#include <QPointer>
#include <QRegularExpression>
#include <QWidget>

class QTreeWidget;
class QTreeWidgetItem;
class QLineEdit;

namespace Qth
{
	class CaptureDlgMgr;
	class TreeInfoWidgetHelper;
	class WatchGlobalFocusDlg;
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

	protected slots:
		void onPickerBtnPressed();
		void onFindWidgetBtnPressed();
		void onRefreshBtnPressed();
		void onWatchFocusBtnPressed();
		void onSearchTextChanged(const QString&);
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
		QLineEdit*				m_searchEdit = nullptr;
		bool						m_searching = false;
		QPointer<WatchGlobalFocusDlg>	m_watchGlobalFocusDlg = nullptr;
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
		void setSearchPattern(const QString& search);

	signals:
		void sigHighLightWidget(QWidget* widget);

	private:
		void initUI();
		void initSignalConnect();
		void setItemInfo(QTreeWidgetItem* item, QWidget* node);
		void addChildInfo(QTreeWidgetItem* parentItem, QWidget* parentWidget, bool recursion = true);
		int itemDepth(QTreeWidgetItem* parentItem);
		void showMenu(QTreeWidgetItem* item, int column);
		void showWidgetInfoDetailed(QWidget* widget);
		void showWatchWidget(QObject* obj);
		QWidget* getItemWidget(QTreeWidgetItem* item, bool showTip = true);
		void filterItems();
		bool filterItemRecursive(QTreeWidgetItem* item);
	private:
		typedef QHash<QTreeWidgetItem*, QPointer<QWidget>> TreeItemToWidget;
		typedef QHash<QWidget*, QTreeWidgetItem*> WidgetToTreeItem;
		typedef QMap<QObject*, QPointer<QWidget>> WatcherList;

		QTreeWidget*			m_treeInfo = nullptr;
		QStringList			m_headerList;
		QMap<QString, int>	m_headerToIndex;
		QTreeWidgetItem*		m_selectItemLast = nullptr;

		TreeItemToWidget		m_treeItemToWidget;
		WidgetToTreeItem		m_widgetToTreeItem;
		QRegularExpression	m_searchRegExp;

		WidgetDetails*		m_widgetDetails = nullptr;
		WatcherList			m_watcherList;
	};
}