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
	class ObjectCollector;
	class TreeInfoObjectHelper;

	//////////////////////////////////////////////////////////////////////////
	// WidgetHelper
	class ObjectHelper : public QWidget
	{
		Q_OBJECT
	public:
		explicit ObjectHelper(QWidget* parent = nullptr);
		~ObjectHelper();

	public:
		static QString getWidgetAttributeString(QWidget* curWidget);

	protected slots:
		void onPickerBtnPressed();
		void onFindWidgetBtnPressed();
		void onRefreshBtnPressed();
		void onTipBtnPressed();
		void onSearchTextChanged(const QString&);
		void onHighLightWidget(QWidget* widget);
		void onCatchWidgetChanged(QWidget* targetWidget);
		void onCatchWidgetFinish(QWidget* targetWidget);

	private:
		void initUI();
		QWidget* createTitle();
		QWidget* createreeInfoWidge();
		void updateCatchObjectInfo(QObject* target, bool force = false);

	private:
		CaptureDlgMgr*			m_captureDlgMgr = nullptr;
		QPointer<QObject>		m_currentCaptureObj;
		TreeInfoObjectHelper*	m_treeInfoObjectHelper = nullptr;
		QLineEdit*				m_searchEdit = nullptr;
		bool						m_searching = false;
	};

	//////////////////////////////////////////////////////////////////////////
	// TreeInfoWidget
	class TreeInfoObjectHelper : public QWidget
	{
		Q_OBJECT
	public:
		explicit TreeInfoObjectHelper(QWidget* parent = nullptr);
		~TreeInfoObjectHelper();

		void clearAllInfo();
		void showAllObjectInfo();
		bool findAndScrollToTargetInTree(QObject* target);
		void setSearchPattern(const QString& search);

	signals:
		void sigHighLightWidget(QWidget* widget);

	protected:
		void showEvent(QShowEvent* event) override;

	private:
		void initUI();
		void initSignalConnect();
		void addObjectRootNode(QObject* root);
		void addChildInfo(QTreeWidgetItem* parentItem, QObject* parent, bool recursion = true);
		void setItemInfo(QTreeWidgetItem* item, QObject* node);
		void showMenu(QTreeWidgetItem* item, int column);
		void showObjectInfoDetailed(QObject* widget);
		void showWatchWidget(QObject* obj);
		QObject* getItemObject(QTreeWidgetItem* item, bool showTip = true);
		QWidget* tryToWidget(QObject* object);
		void filterItems();
		bool filterItemRecursive(QTreeWidgetItem* item);
	private:
		typedef QHash<QTreeWidgetItem*, QPointer<QObject>> TreeItemToObject;
		typedef QHash<QObject*, QTreeWidgetItem*> ObjectToTreeItem;
		typedef QMap<QObject*, QPointer<QWidget>> WatcherList;

		QTreeWidget*			m_treeInfo = nullptr;
		QStringList			m_headerList;
		QMap<QString, int>	m_headerToIndex;
		QTreeWidgetItem*		m_selectItemLast = nullptr;

		TreeItemToObject		m_treeItemToObject;
		ObjectToTreeItem		m_objectToTreeItem;
		QRegularExpression	m_searchRegExp;
		WatcherList			m_watcherList;
		ObjectCollector*		m_objectCollector = nullptr;
		bool					m_firstShowObjectTree = true;
	};
}