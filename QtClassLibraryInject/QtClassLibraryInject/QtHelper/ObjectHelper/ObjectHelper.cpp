#include "ObjectHelper.h"
#include "ObjectCollector.h"
#include "../CaptureDlg.h"
#include "../Common.h"
#include "../ObjectHelper/WatchObjectWidget.h"
#include "WatchGlobalFocusDlg.h"

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QQueue>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWindow>

namespace Qth
{
	const QString Name_Addr			= "addrObject";
	const QString Name_Class		= "classObject";
	const QString Name_ObjectName	= "objectNameObject";
	const QString Name_ObjectDetail = "objectDetail";

	//////////////////////////////////////////////////////////////////////////
	// WidgetHelper
	ObjectHelper::ObjectHelper(QWidget* parent/* = nullptr*/)
		: QWidget(parent)
	{
		m_captureDlgMgr = new CaptureDlgMgr();
		m_captureDlgMgr->seCaptureMode(CaptureDlgMgr::All);
		connect(m_captureDlgMgr, &CaptureDlgMgr::sigCatchWidgetChanged, this, &ObjectHelper::onCatchWidgetChanged);
		connect(m_captureDlgMgr, &CaptureDlgMgr::sigCatchWidgetFinish, this, &ObjectHelper::onCatchWidgetFinish);

		initUI();
	}
	
	ObjectHelper::~ObjectHelper()
	{
	}

	void ObjectHelper::initUI()
	{
		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->setSpacing(0);
		mainLayout->setContentsMargins(0, 0, 0, 0);
		mainLayout->addWidget(createTitle());
		mainLayout->addWidget(createreeInfoWidge());
	}

	QWidget* ObjectHelper::createTitle()
	{
		QWidget* widget = new QWidget(this);
		widget->setFixedHeight(25);
		QHBoxLayout* hLayout = new QHBoxLayout(widget);
		hLayout->setSpacing(0);
		hLayout->setContentsMargins(12, 5, 12, 0);

		QPushButton* pickerBtn = new QPushButton(this);
		pickerBtn->setAutoRepeat(true);
		pickerBtn->setFixedSize(QSize(16, 16));
		pickerBtn->setFocusPolicy(Qt::NoFocus);

		QLabel* addrLabel = new QLabel("Address:", this);
		addrLabel->setFixedWidth(Common::getContentWidth(addrLabel->font(), "Address:").width() + 8);
		QLineEdit* addrEdit = new QLineEdit(this);
		addrEdit->setObjectName(Name_Addr);
		addrEdit->setFixedWidth(80);

		QLabel* classLabel = new QLabel("ClassName:", this);
		classLabel->setFixedWidth(Common::getContentWidth(classLabel->font(), "ClassName:").width() + 8);
		QLineEdit* classEdit = new QLineEdit(this);
		classEdit->setObjectName(Name_Class);
		classEdit->setFixedWidth(120);

		QPushButton* findObjectBtn = new QPushButton("SearchObject", this);
		findObjectBtn->setFixedWidth(Common::getContentWidth(findObjectBtn->font(), "SearchWidget").width() + 20);

		QPushButton* refreshBtn = new QPushButton("RefreshTree", this);
		refreshBtn->setFixedWidth(Common::getContentWidth(refreshBtn->font(), "RefreshTree").width() + 20);

		QPushButton* watchQObjectFocusBtn = new QPushButton("QObjectFocus", this);
		watchQObjectFocusBtn->setFixedWidth(Common::getContentWidth(watchQObjectFocusBtn->font(), "QObjectFocus").width() + 20);

		QPushButton* watchQWindowFocusBtn = new QPushButton("QWindowFocus", this);
		watchQWindowFocusBtn->setFixedWidth(Common::getContentWidth(watchQWindowFocusBtn->font(), "QWindowFocus").width() + 20);

		m_searchEdit = new QLineEdit(this);
		m_searchEdit->setObjectName(Name_Class);
		m_searchEdit->setFixedWidth(200);
		m_searchEdit->setPlaceholderText("search object");

		QPushButton* tipBtn = new QPushButton(QApplication::style()->standardIcon((QStyle::SP_MessageBoxQuestion)), "", this);
		tipBtn->setFixedSize(QSize(16, 16));
		tipBtn->setFocusPolicy(Qt::NoFocus);

		hLayout->addWidget(pickerBtn);
		hLayout->addSpacing(10);
		hLayout->addWidget(addrLabel);
		hLayout->addWidget(addrEdit);
		hLayout->addSpacing(10);
		hLayout->addWidget(classLabel);
		hLayout->addWidget(classEdit);
		hLayout->addSpacing(10);
		hLayout->addWidget(findObjectBtn);
		hLayout->addSpacing(10);
		hLayout->addWidget(refreshBtn);
		hLayout->addSpacing(10);
		hLayout->addWidget(watchQObjectFocusBtn);
		hLayout->addSpacing(10);
		hLayout->addWidget(watchQWindowFocusBtn);
		hLayout->addSpacing(10);
		hLayout->addWidget(m_searchEdit);
		hLayout->addSpacing(10);
		hLayout->addStretch();
		hLayout->addWidget(tipBtn);

		connect(pickerBtn, &QPushButton::pressed, this, &ObjectHelper::onPickerBtnPressed);
		connect(findObjectBtn, &QPushButton::pressed, this, &ObjectHelper::onFindWidgetBtnPressed);
		connect(refreshBtn, &QPushButton::pressed, this, &ObjectHelper::onRefreshBtnPressed);
		connect(tipBtn, &QPushButton::pressed, this, &ObjectHelper::onTipBtnPressed);
		connect(watchQObjectFocusBtn, &QPushButton::pressed, this, &ObjectHelper::onWatchQObjectFocusBtnPressed);
		connect(watchQWindowFocusBtn, &QPushButton::pressed, this, &ObjectHelper::onWatchQWindowFocusBtnPressed);
		connect(m_searchEdit, &QLineEdit::textChanged, this, &ObjectHelper::onSearchTextChanged);

		return widget;
	}

	void ObjectHelper::onPickerBtnPressed()
	{
		if (!m_captureDlgMgr)
			return;

		setCursor(Qt::CrossCursor);
		m_captureDlgMgr->startAutoPickerWidget();
	}
	
	void ObjectHelper::onFindWidgetBtnPressed()
	{
		QObject* curObj = nullptr;
		do 
		{
			QLineEdit* lineEdit = findChild<QLineEdit*>(Name_Addr);
			if (!lineEdit)
				break;

			QString addrString = lineEdit->text();
			if (addrString.isEmpty())
				break;

			bool ret = false;
			quint64 addr = addrString.toULongLong(&ret, 16);
			if (!ret)
				break;

			curObj = Common::convertAddrToObject(addr);

		} while (false);

		if (!curObj)
		{
			QMessageBox::information(this, "Warning", "Address is invalid!");
			return;
		}

		if (m_treeInfoObjectHelper)
			m_treeInfoObjectHelper->findAndScrollToTargetInTree(curObj);

		updateCatchObjectInfo(curObj, true);
	}

	void ObjectHelper::onRefreshBtnPressed()
	{
		if (!m_treeInfoObjectHelper)
			return;

		m_treeInfoObjectHelper->showAllObjectInfo();
	}

	void ObjectHelper::onTipBtnPressed()
	{
		QMessageBox::information(this, "Object", "The QObject objects collected are not complete!\nGammRay also has the same problem.");
	}

	void ObjectHelper::onWatchQObjectFocusBtnPressed()
	{
		if (!m_watchQObjectFocusDlg)
		{
			m_watchQObjectFocusDlg = new WatchGlobalFocusDlg(this, WatchGlobalFocusDlg::WatchType::QObjectFocus);
			m_watchQObjectFocusDlg->setAttribute(Qt::WA_DeleteOnClose);
		}

		m_watchQObjectFocusDlg->showNormal();
		m_watchQObjectFocusDlg->raise();
	}

	void ObjectHelper::onWatchQWindowFocusBtnPressed()
	{
		if (!m_watchQWindowFocusDlg)
		{
			m_watchQWindowFocusDlg = new WatchGlobalFocusDlg(this, WatchGlobalFocusDlg::WatchType::QWindowFocus);
			m_watchQWindowFocusDlg->setAttribute(Qt::WA_DeleteOnClose);
		}

		m_watchQWindowFocusDlg->showNormal();
		m_watchQWindowFocusDlg->raise();
	}

	void ObjectHelper::onSearchTextChanged(const QString&)
	{
		if (m_searching)
			return;
		m_searching = true;

		QTimer::singleShot(100, this, [=]() {
			if (m_treeInfoObjectHelper)
				m_treeInfoObjectHelper->setSearchPattern(m_searchEdit->text());
			m_searching = false;
		});
	}

	QWidget* ObjectHelper::createreeInfoWidge()
	{
		if (!m_treeInfoObjectHelper)
		{
			m_treeInfoObjectHelper = new TreeInfoObjectHelper(this);
			connect(m_treeInfoObjectHelper, &TreeInfoObjectHelper::sigHighLightWidget, this, &ObjectHelper::onHighLightWidget);
		}
			
		return m_treeInfoObjectHelper;
	}

	void ObjectHelper::onHighLightWidget(QObject* widget)
	{
		if (!widget)
			return;

		if (m_captureDlgMgr)
			m_captureDlgMgr->highLightWidget(widget);
	}

	void ObjectHelper::onCatchWidgetChanged(QObject* target)
	{
		updateCatchObjectInfo(target);
	}

	void ObjectHelper::onCatchWidgetFinish(QObject* target)
	{
		setCursor(Qt::ArrowCursor);
		updateCatchObjectInfo(target);

		if (m_treeInfoObjectHelper && target)
		{
			if (!m_treeInfoObjectHelper->findAndScrollToTargetInTree(target))
				QMessageBox::warning(this, "warning", "Can't find target object in object tree! Try refresh object tree.");
		}
	}

	void ObjectHelper::updateCatchObjectInfo(QObject* target, bool force/* = false*/)
	{
		if (m_currentCaptureObj == target && !force)
			return;
		m_currentCaptureObj = target;

		// 地址
		QLineEdit* lineEdit = findChild<QLineEdit*>(Name_Addr);
		if (lineEdit)
		{
			QString addr;
			if (m_currentCaptureObj)
				addr = QString("0x%0").arg(QString::number((quint64)m_currentCaptureObj.data(), 16));
			lineEdit->setText(addr);
		}

		// 类名
		lineEdit = findChild<QLineEdit*>(Name_Class);
		if (lineEdit)
		{
			QString className;
			if (m_currentCaptureObj)
				className = QString::fromStdString(m_currentCaptureObj->metaObject()->className());
			lineEdit->setText(className);
		}
	}

	QString ObjectHelper::getWidgetAttributeString(QWidget* curWidget)
	{
		QString ret;
		if (!curWidget)
			return ret;

		const int maxAttributeIndex = (int)Qt::WA_AttributeCount;
		for (int index = 0; index < maxAttributeIndex; ++index)
		{
			if (curWidget->testAttribute((Qt::WidgetAttribute)index))
			{
				if (!ret.isEmpty())
				{
					ret += " | ";
				}
				ret += QString::number(index);
			}
		}
		return ret;
	}

	//////////////////////////////////////////////////////////////////////////
	// TreeInfoObjectHelper
	TreeInfoObjectHelper::TreeInfoObjectHelper(QWidget* parent)
		: QWidget(parent)
		, m_objectCollector(new ObjectCollector(this))
	{
		m_objectCollector->init();
		initUI();
		initSignalConnect();
		m_searchRegExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	}

	TreeInfoObjectHelper::~TreeInfoObjectHelper()
	{
	}

	void TreeInfoObjectHelper::initUI()
	{
		typedef QPair<QString, QString> StringPair;
		QList<StringPair> headerString;
		headerString.append(StringPair(Name_Class, "ClassName"));
		headerString.append(StringPair(Name_Addr, "Address"));
		headerString.append(StringPair(Name_ObjectName, "ObjectName"));
		headerString.append(StringPair(Name_ObjectDetail, "ObjectDetail"));

		for (int i = 0; i < headerString.size(); i++)
		{
			m_headerToIndex.insert(headerString[i].first, i);
			m_headerList.append(headerString[i].second);
		}

		m_treeInfo = new QTreeWidget(this);
		m_treeInfo->setExpandsOnDoubleClick(false);
		m_treeInfo->setHeaderLabels(m_headerList);
		m_treeInfo->header()->setSectionResizeMode(m_headerToIndex[Name_Class], QHeaderView::ResizeToContents);

		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->addWidget(m_treeInfo);
	}

	void TreeInfoObjectHelper::initSignalConnect()
	{
		auto highLightFunc = [=](QTreeWidgetItem* item) {
			QObject* obj = getItemObject(item);
			if (!obj)
				return;
			emit sigHighLightWidget(obj);
		};

		connect(m_treeInfo, &QTreeWidget::itemClicked, this, [=](QTreeWidgetItem* item, int column) {
			highLightFunc(item);
		});

		connect(m_treeInfo, &QTreeWidget::itemSelectionChanged, this, [=]() {
			QTreeWidgetItem* curItem = m_treeInfo->currentItem();
			if (m_selectItemLast == curItem)
				return;
			m_selectItemLast = curItem;

			highLightFunc(m_selectItemLast);
		});

		connect(m_treeInfo, &QTreeWidget::itemDoubleClicked, this, [=](QTreeWidgetItem* item, int column) {
			if (!item)
				return;
			if (QObject* obj = getItemObject(item))
				showObjectInfoDetailed(obj);
		});

		connect(m_treeInfo, &QTreeWidget::itemPressed, this, [=](QTreeWidgetItem* item, int column) {
			showMenu(item, column);
		});
	}

	void TreeInfoObjectHelper::showMenu(QTreeWidgetItem* item, int column)
	{
		if (!item)
			return;

		if (QApplication::mouseButtons() != Qt::RightButton)
			return;

		QClipboard* clip = QApplication::clipboard();

		QMenu* menu = new QMenu(this);
		menu->addAction("copy item", [&]() {
			QString coltext = item->text(column);
			clip->setText(coltext);
		});
		menu->addAction("copy row item", [&]() {
			QString text;
			for (int i = 0; i < item->columnCount(); i++)
			{
				text += item->text(i);
				if (i != item->columnCount())
					text += "  ";
			}
			clip->setText(text);
		});
		menu->addAction("show object detailed", [&]() {
			QTimer::singleShot(10, this, [=]() {
				if (QObject* obj = getItemObject(item))
					showObjectInfoDetailed(obj);
			});
		});
		menu->addAction("watch this object", [&]() {
			QTimer::singleShot(10, this, [=]() {
				if (QObject* obj = getItemObject(item))
					showWatchWidget(obj);
			});
		});
		menu->exec(QCursor::pos());
		menu->deleteLater();
	}

	void TreeInfoObjectHelper::clearAllInfo()
	{
		m_treeInfo->clear();
		m_treeItemToObject.clear();
		m_objectToTreeItem.clear();
	}

	void TreeInfoObjectHelper::addObjectRootNode(QObject* root)
	{
		if (!root)
			return;

		QTreeWidgetItem* itemTop = new QTreeWidgetItem();
		setItemInfo(itemTop, root);
		m_treeItemToObject[itemTop] = root;
		m_objectToTreeItem[root] = itemTop;

		m_treeInfo->addTopLevelItem(itemTop);

		addChildInfo(itemTop, root);
	}

	void TreeInfoObjectHelper::showAllObjectInfo()
	{
		clearAllInfo();
		if (!m_objectCollector)
			return;

		QSet<QObject*> objects = m_objectCollector->topObjects();
		for (auto i : objects)
		{
			addObjectRootNode(i);
		}

		if (!m_searchRegExp.pattern().isEmpty())
			filterItems();
	}

	bool TreeInfoObjectHelper::findAndScrollToTargetInTree(QObject* target)
	{
		if (!m_treeInfo)
			return false;

		auto it = m_objectToTreeItem.find(target);
		if (it == m_objectToTreeItem.end())
			return false;

		m_treeInfo->scrollToItem(it.value());
		QItemSelectionModel* selectionModel = m_treeInfo->selectionModel();
		if (selectionModel)
			selectionModel->clearSelection();
		m_treeInfo->setItemSelected(it.value(), true);

		return true;
	}

	void TreeInfoObjectHelper::addChildInfo(QTreeWidgetItem* parentItem, QObject* parentWidget, bool recursion/* = true*/)
	{
		if (!parentItem || !parentWidget)
			return;

		typedef QPair<QTreeWidgetItem*, QObject*> ItemObjectPair;
		QQueue<ItemObjectPair> queue;
		queue.enqueue(ItemObjectPair(parentItem, parentWidget));

		while (queue.size() > 0)
		{
			ItemObjectPair itemWidget = queue.dequeue();
			const QObjectList& objList = itemWidget.second->children();
			for (int i = 0; i < objList.size(); i++)
			{
				QTreeWidgetItem* item = new QTreeWidgetItem();
				setItemInfo(item, objList[i]);
				m_treeItemToObject[item] = objList[i];
				m_objectToTreeItem[objList[i]] = item;

				itemWidget.first->addChild(item);

				if (recursion)
					queue.enqueue(ItemObjectPair(item, objList[i]));
			}
		}
	}

	void TreeInfoObjectHelper::setItemInfo(QTreeWidgetItem* item, QObject* node)
	{
		if (!node || !item)
			return;

		item->setText(m_headerToIndex[Name_Addr], QString("0x%0").arg(QString::number((quint64)node, 16)));
		item->setText(m_headerToIndex[Name_Class], QString::fromStdString(node->metaObject()->className()));
		item->setText(m_headerToIndex[Name_ObjectName], QString("%0").arg(node->objectName()));

		QString output;
		QDebug debug(&output);
		debug.setVerbosity(QDebug::MaximumVerbosity);
		if (QWidget* widget = qobject_cast<QWidget*>(node))
			debug << widget;
		else if (QWindow* window = qobject_cast<QWindow*>(node))
			debug << window;
		else
			debug << node;
		item->setText(m_headerToIndex[Name_ObjectDetail], output);
	}

	void TreeInfoObjectHelper::showObjectInfoDetailed(QObject* widget)
	{
	}

	void TreeInfoObjectHelper::showWatchWidget(QObject* obj)
	{
		if (!obj)
			return;

		auto itFind = m_watcherList.find(obj);
		if (itFind != m_watcherList.end() && itFind.value())
		{
			itFind.value()->showNormal();
			return;
		}

		WatchObjectWidget* widget = new WatchObjectWidget(this);
		m_watcherList[obj] = widget;
		widget->setAttribute(Qt::WA_DeleteOnClose);
		widget->setFilterObject(obj);
		widget->showNormal();
		connect(widget, &WatchObjectWidget::sigHighlight, this, [=](QObject* obj) {
			emit sigHighLightWidget(obj);
		});
	}

	QObject* TreeInfoObjectHelper::getItemObject(QTreeWidgetItem* item, bool showTip)
	{
		auto find = m_treeItemToObject.find(item);
		if (find == m_treeItemToObject.end())
			return false;

		QObject* widget = find.value();
		if (!widget && showTip)
			QMessageBox::warning(this, "warning", "The object had destroyed! Try refresh object tree.");

		return widget;

	}

	QWidget* TreeInfoObjectHelper::tryToWidget(QObject* object)
	{
		return qobject_cast<QWidget*>(object);
	}

	void TreeInfoObjectHelper::showEvent(QShowEvent* event)
	{
		QWidget::showEvent(event);

		if (m_firstShowObjectTree)
		{
			m_firstShowObjectTree = false;
			showAllObjectInfo();
		}
	}

	void TreeInfoObjectHelper::setSearchPattern(const QString& search)
	{
		QString patternOld = m_searchRegExp.pattern();
		if (patternOld == search)
			return;
		m_searchRegExp.setPattern(search);

		filterItems();
	}

	void TreeInfoObjectHelper::filterItems()
	{
		if (!m_treeInfo)
			return;

		QTreeWidgetItem* root = m_treeInfo->invisibleRootItem();
		for (int i = 0; i < root->childCount(); i++)
		{
			filterItemRecursive(root->child(i));
		}
	}

	bool TreeInfoObjectHelper::filterItemRecursive(QTreeWidgetItem* item)
	{
		bool show = false;
		bool childShow = false;
		do 
		{
			for (int i = 0; i < item->childCount(); i++)
			{
				childShow |= filterItemRecursive(item->child(i));
			}
			if (childShow)
				break;
	
			QString text = item->text(m_headerToIndex[Name_Class]);
			show = m_searchRegExp.match(text).hasMatch();
			if (show)
				break;

			text = item->text(m_headerToIndex[Name_ObjectName]);
			show = m_searchRegExp.match(text).hasMatch();
		} while (false);

		show |= childShow;
		item->setHidden(!show);
		if (childShow && !m_searchRegExp.pattern().isEmpty())
			item->setExpanded(true);

		return show;
	}
}