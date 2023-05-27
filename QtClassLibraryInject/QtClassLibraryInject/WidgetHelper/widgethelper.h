#pragma once

#include <QDialog>
#include <QGridLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QPointer>
#include <QTimer>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

class QTreeWidget;
class QTreeWidgetItem;
class TreeInfoWidgetHelper;
class WidgetInfoModifier;

class WidgetHelper : public QDialog
{
	Q_OBJECT
public:
	explicit WidgetHelper(QWidget* parent = nullptr);
	~WidgetHelper();

public:
	static QString getWidgetAttributeString(QWidget* curWidget);
	static QWidget* convertAddrToWidget(quint64 addr);

private:
	bool createItem(const QString &caption, const QString &name, QGridLayout *mainLayout);
	bool createShowHideBtn(const QString &caption, const QString &name, QGridLayout *mainLayout);
	bool createHighLightBtn(QVBoxLayout *mainLayout);
	bool createSearchBtn(QVBoxLayout *mainLayout);
	bool createTreeInfoBtn(QVBoxLayout* mainLayout);
	void reset();
	void clearWidgetData();
	void updateWidgetInfo(QWidget *curWidget);
	void updateCaptureDlgPos(QWidget *widget);
	void hideCaptureDlg();
protected slots:
	void onCheckTimeout();
	void onHighLightWidget(QWidget* widget);
private:
	QTimer*		m_checkWidgetTimer = nullptr;
	QTimer*		m_highLightTimer = nullptr;
	int			m_highLightCount = 0;
	QWidget*		m_highLightWidget = nullptr;

	QPointer<QWidget>	m_preWidget;
	QPointer<QWidget>	m_curWidget;
#ifdef Q_OS_WIN
	bool createCaptureDlg();
	static LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	HWND	m_hWnd = nullptr;
#endif
	QPointer<QWindow> m_CaptureWindow;

	TreeInfoWidgetHelper* m_treeInfoWidget = nullptr;
};

//////////////////////////////////////////////////////////////////////////
// TreeInfoWidget
class TreeInfoWidgetHelper : public QDialog
{
	Q_OBJECT
public:
	explicit TreeInfoWidgetHelper(QWidget* parent = nullptr);
	~TreeInfoWidgetHelper();

	void setWidgetRootNode(QWidget* root);

signals:
	void sigHighLightWidget(QWidget* widget);

private:
	void initSignalConnect();
	void setItemInfo(QTreeWidgetItem* item, QWidget* node);
	void addChildInfo(QTreeWidgetItem* parentItem, QWidget* parentWidget, bool recursion = true);
	int itemDepth(QTreeWidgetItem* parentItem);
	void showWidgetInfoModifier(QWidget* widget);
private:
	typedef QHash<QTreeWidgetItem*, QWidget*> TreeItemToWidget;
	typedef QHash<QWidget*, QTreeWidgetItem*> WidgetToTreeItem;

	QTreeWidget*		m_treeInfo = nullptr;
	QStringList			m_headerList;
	QMap<QString, int>	m_headerToIndex;
	QTreeWidgetItem*	m_selectItemLast = nullptr;

	TreeItemToWidget		m_treeItemToWidget;
	WidgetToTreeItem		m_widgetToTreeItem;

	WidgetInfoModifier* m_widgetInfoModifier = nullptr;
};

//////////////////////////////////////////////////////////////////////////
// WidgetInfoModifier
class WidgetInfoModifier : public QDialog
{
	Q_OBJECT
public:
	explicit WidgetInfoModifier(QWidget* parent = nullptr);
	~WidgetInfoModifier();
	void setTargetWidget(QWidget* widget);

signals:
	void sigUpdateWidget(QWidget* widget);

private:
	void initUI();
	QWidget* addBaseInfoWidget();
	QWidget* addStyleSheetWidget();

	//void update

	void applyAllWidgetInfo();
	void updateAllWidgetInfo();

private:
	QTabWidget*			m_tabWidget = nullptr;
	QPointer<QWidget>	m_targetWidget;
};