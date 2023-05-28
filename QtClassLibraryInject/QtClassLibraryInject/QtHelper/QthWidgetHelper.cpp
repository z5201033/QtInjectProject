#include "QthWidgetHelper.h"
#include "QthCaptureDlg.h"

#include <QApplication>
#include <QClipboard>
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

namespace Qth
{
	const QString Name_PickBtn		= "pickBtnObject";
	const QString Name_Addr			= "addrObject";
	const QString Name_Class			= "classObject";
	const QString Name_ObjectName	= "objectNameObject";
	const QString Name_Rect			= "rectObject";
	const QString Name_Width			= "widthObject";
	const QString Name_Height		= "heightObject";
	const QString Name_Visible		= "visibleObject";
	const QString Name_Attribute		= "attributeObject";
	const QString Name_WindowFlags	= "windowFlagsObject";
	const QString Name_StyleSheet	= "stylesheetObject";
	const QString Name_ParentWidget	= "parentWidgetObject";
	const QString Name_ParentAddr	= "parentAddrObject";

	static int getContentWidth(QFont font, QString str)
	{
		QFontMetrics fontMetrics(font);
		QRect rect = fontMetrics.boundingRect(str);
		return rect.width();
	}

	//////////////////////////////////////////////////////////////////////////
	// WidgetHelper
	WidgetHelper::WidgetHelper(QWidget* parent/* = nullptr*/)
		: QWidget(parent)
	{
		m_captureDlgMgr = new CaptureDlgMgr();
		connect(m_captureDlgMgr, &CaptureDlgMgr::sigCatchWidgetChanged, this, &WidgetHelper::onCatchWidgetChanged);
		connect(m_captureDlgMgr, &CaptureDlgMgr::sigCatchWidgetFinish, this, &WidgetHelper::onCatchWidgetFinish);

		initUI();

		QTimer::singleShot(100, this, [=] {
			if (m_treeInfoWidgetHelper)
				m_treeInfoWidgetHelper->showAllWidgetInfo();
		});
	}
	
	WidgetHelper::~WidgetHelper()
	{
	}

	void WidgetHelper::initUI()
	{
		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->setSpacing(0);
		mainLayout->setContentsMargins(0, 0, 0, 0);
		mainLayout->addWidget(createTitle());
		mainLayout->addWidget(createreeInfoWidge());
	}

	QWidget* WidgetHelper::createTitle()
	{
		QWidget* widget = new QWidget(this);
		widget->setFixedHeight(25);
		QHBoxLayout* hLayout = new QHBoxLayout(widget);
		hLayout->setSpacing(0);
		hLayout->setContentsMargins(12, 5, 0, 0);

		QPushButton* pickerBtn = new QPushButton(this);
		pickerBtn->setObjectName(Name_PickBtn);
		pickerBtn->setAutoRepeat(true);
		pickerBtn->setFixedSize(QSize(16, 16));
		pickerBtn->setFocusPolicy(Qt::NoFocus);

		QLabel* addrLabel = new QLabel("Address:", this);
		addrLabel->setFixedWidth(getContentWidth(addrLabel->font(), "Address:") + 8);
		QLineEdit* addrEdit = new QLineEdit(this);
		addrEdit->setObjectName(Name_Addr);
		addrEdit->setFixedWidth(80);

		QLabel* classLabel = new QLabel("ClassName:", this);
		classLabel->setFixedWidth(getContentWidth(classLabel->font(), "ClassName:") + 8);
		QLineEdit* classEdit = new QLineEdit(this);
		classEdit->setObjectName(Name_Class);
		classEdit->setFixedWidth(120);

		QPushButton* findWidgetBtn = new QPushButton("SearchWidget", this);
		findWidgetBtn->setFixedWidth(getContentWidth(findWidgetBtn->font(), "SearchWidget") + 8);

		QPushButton* refreshBtn = new QPushButton("RefreshTree", this);
		refreshBtn->setFixedWidth(getContentWidth(refreshBtn->font(), "RefreshTree") + 8);

		hLayout->addWidget(pickerBtn);
		hLayout->addSpacing(10);
		hLayout->addWidget(addrLabel);
		hLayout->addWidget(addrEdit);
		hLayout->addSpacing(10);
		hLayout->addWidget(classLabel);
		hLayout->addWidget(classEdit);
		hLayout->addSpacing(10);
		hLayout->addWidget(findWidgetBtn);
		hLayout->addSpacing(10);
		hLayout->addWidget(refreshBtn);
		hLayout->addSpacing(10);
		hLayout->addStretch();

		connect(pickerBtn, &QPushButton::pressed, this, &WidgetHelper::onPickerBtnPressed);
		connect(findWidgetBtn, &QPushButton::pressed, this, &WidgetHelper::onFindWidgetBtnPressed);
		connect(refreshBtn, &QPushButton::pressed, this, &WidgetHelper::onRefreshBtnPressed);

		return widget;
	}

	void WidgetHelper::onPickerBtnPressed()
	{
		if (!m_captureDlgMgr)
			return;

		setCursor(Qt::CrossCursor);
		m_captureDlgMgr->startAutoPickerWidget();
	}
	
	void WidgetHelper::onFindWidgetBtnPressed()
	{
		QWidget* curWidget = nullptr;
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

			curWidget = convertAddrToWidget(addr);

		} while (false);

		if (!curWidget)
		{
			QMessageBox::information(this, "Warning", "Address is invalid!");
			return;
		}

		if (m_treeInfoWidgetHelper && curWidget)
			m_treeInfoWidgetHelper->findAndScrollToTargetInTree(curWidget);

		updateCatchWidgetInfo(curWidget, true);
	}

	void WidgetHelper::onRefreshBtnPressed()
	{
		if (!m_treeInfoWidgetHelper)
			return;

		m_treeInfoWidgetHelper->showAllWidgetInfo();
	}

	QWidget* WidgetHelper::createreeInfoWidge()
	{
		if (!m_treeInfoWidgetHelper)
		{
			m_treeInfoWidgetHelper = new TreeInfoWidgetHelper(this);
			connect(m_treeInfoWidgetHelper, &TreeInfoWidgetHelper::sigHighLightWidget, this, &WidgetHelper::onHighLightWidget);
		}
			
		return m_treeInfoWidgetHelper;
	}

	QWidget* WidgetHelper::convertAddrToWidget(quint64 addr)
	{
		try
		{
			QWidget* curWidget = qobject_cast<QWidget*>((QObject*)addr);
			if (!curWidget)
			{
				return nullptr;
			}

			return curWidget;
		}
		catch (...)
		{
			return nullptr;
		}
	}

	void WidgetHelper::onHighLightWidget(QWidget* widget)
	{
		if (!widget)
			return;

		if (m_captureDlgMgr)
			m_captureDlgMgr->highLightWidget(widget);
	}

	void WidgetHelper::onCatchWidgetChanged(QWidget* targetWidget)
	{
		updateCatchWidgetInfo(targetWidget);
	}

	void WidgetHelper::onCatchWidgetFinish(QWidget* targetWidget)
	{
		setCursor(Qt::ArrowCursor);
		updateCatchWidgetInfo(targetWidget);

		if (m_treeInfoWidgetHelper && targetWidget)
		{
			if (!m_treeInfoWidgetHelper->findAndScrollToTargetInTree(targetWidget))
				QMessageBox::information(this, "warning", "Can't find target widget in widget tree! Try refresh widget tree.");
		}
	}

	void WidgetHelper::updateCatchWidgetInfo(QWidget* targetWidget, bool force/* = false*/)
	{
		if (m_currentCaptureWidget == targetWidget && !force)
			return;
		m_currentCaptureWidget = targetWidget;

		// 地址
		QLineEdit* lineEdit = findChild<QLineEdit*>(Name_Addr);
		if (lineEdit)
		{
			QString addr;
			if (m_currentCaptureWidget)
				addr = QString("0x%0").arg(QString::number((quint64)m_currentCaptureWidget.data(), 16));
			lineEdit->setText(addr);
		}

		// 类名
		lineEdit = findChild<QLineEdit*>(Name_Class);
		if (lineEdit)
		{
			QString className;
			if (m_currentCaptureWidget)
				className = QString::fromStdString(m_currentCaptureWidget->metaObject()->className());
			lineEdit->setText(className);
		}
	}

	QString WidgetHelper::getWidgetAttributeString(QWidget* curWidget)
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
	// TreeInfoWidget
	TreeInfoWidgetHelper::TreeInfoWidgetHelper(QWidget* parent)
		: QWidget(parent)
	{
		typedef QPair<QString, QString> StringPair;
		QList<StringPair> headerString;
		headerString.append(StringPair(Name_Class, "ClassName"));
		headerString.append(StringPair(Name_Addr, "Address"));
		headerString.append(StringPair(Name_ObjectName, "ObjectName"));
		headerString.append(StringPair(Name_Rect, "Rect"));
		headerString.append(StringPair(Name_Visible, "Visible"));
		headerString.append(StringPair(Name_Attribute, "Attributes"));
		headerString.append(StringPair(Name_WindowFlags, "WindowFlags"));
		headerString.append(StringPair(Name_StyleSheet, "StyleSheet"));

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

		initSignalConnect();
	}

	TreeInfoWidgetHelper::~TreeInfoWidgetHelper()
	{
	}

	void TreeInfoWidgetHelper::initSignalConnect()
	{
		auto highLightFunc = [=](QTreeWidgetItem* item) {
			if (!item)
				return;

			QString addrString = item->text(m_headerToIndex[Name_Addr]);
			quint64 addr = addrString.toULongLong(nullptr, 16);
			QWidget* curWidget = WidgetHelper::convertAddrToWidget(addr);
			if (curWidget)
				emit sigHighLightWidget(curWidget);
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

			showWidgetInfoDetialed(m_treeItemToWidget[item]);
		});

		connect(m_treeInfo, &QTreeWidget::itemPressed, this, [=](QTreeWidgetItem* item, int column) {
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
		menu->exec(QCursor::pos());
		menu->deleteLater();
			});
	}

	void TreeInfoWidgetHelper::clearAllInfo()
	{
		m_treeInfo->clear();
		m_treeItemToWidget.clear();
		m_widgetToTreeItem.clear();
	}

	void TreeInfoWidgetHelper::setWidgetRootNode(QWidgetList rootList)
	{
		clearAllInfo();

		for (int i = 0; i < rootList.size(); i++)
		{
			addWidgetRootNode(rootList[i]);
		}
	}

	void TreeInfoWidgetHelper::addWidgetRootNode(QWidget* root)
	{
		if (!root)
			return;

		QTreeWidgetItem* itemTop = new QTreeWidgetItem();
		setItemInfo(itemTop, root);
		m_treeItemToWidget[itemTop] = root;
		m_widgetToTreeItem[root] = itemTop;

		m_treeInfo->addTopLevelItem(itemTop);

		addChildInfo(itemTop, root);
	}

	void TreeInfoWidgetHelper::showAllWidgetInfo()
	{
		clearAllInfo();

		QWidgetList allWidgets = QApplication::allWidgets();
		for (int i = 0; i < allWidgets.size(); i++)
		{
			if (!allWidgets[i]->parentWidget())
				addWidgetRootNode(allWidgets[i]);
		}
	}

	bool TreeInfoWidgetHelper::findAndScrollToTargetInTree(QWidget* target)
	{
		if (!m_treeInfo)
			return false;

		WidgetToTreeItem::iterator it = m_widgetToTreeItem.find(target);
		if (it == m_widgetToTreeItem.end())
			return false;

		m_treeInfo->scrollToItem(it.value());
		QItemSelectionModel* selectionModel = m_treeInfo->selectionModel();
		if (selectionModel)
			selectionModel->clearSelection();
		m_treeInfo->setItemSelected(it.value(), true);

		return true;
	}

	void TreeInfoWidgetHelper::addChildInfo(QTreeWidgetItem* parentItem, QWidget* parentWidget, bool recursion/* = true*/)
	{
		if (!parentItem || !parentWidget)
			return;

		typedef QPair<QTreeWidgetItem*, QWidget*> ItemWidgetPair;
		QQueue<ItemWidgetPair> queue;
		queue.enqueue(ItemWidgetPair(parentItem, parentWidget));

		while (queue.size() > 0)
		{
			ItemWidgetPair itemWidget = queue.dequeue();
			QList<QWidget*> widgetList;
			QObjectList objList = itemWidget.second->children();
			for (int i = 0; i < objList.size(); i++)
			{
				QWidget* w = qobject_cast<QWidget*>(objList[i]);
				if (!w)
					continue;

				widgetList.push_back(w);
			}

			for (int i = 0; i < widgetList.size(); i++)
			{
				QTreeWidgetItem* item = new QTreeWidgetItem();
				setItemInfo(item, widgetList[i]);
				m_treeItemToWidget[item] = widgetList[i];
				m_widgetToTreeItem[widgetList[i]] = item;

				itemWidget.first->addChild(item);

				if (recursion)
					queue.enqueue(ItemWidgetPair(item, widgetList[i]));
			}

// 			if (itemWidget.first->childCount() > 0 && itemDepth(itemWidget.first) < 6)
// 				itemWidget.first->setExpanded(true);
		}
	}

	int TreeInfoWidgetHelper::itemDepth(QTreeWidgetItem* parentItem)
	{
		int depth = 0;
		while (parentItem)
		{
			depth++;
			parentItem = parentItem->parent();
		}

		return depth;
	}

	void TreeInfoWidgetHelper::setItemInfo(QTreeWidgetItem* item, QWidget* node)
	{
		if (!node || !item)
			return;

		item->setText(m_headerToIndex[Name_Addr], QString("0x%0").arg(QString::number((quint64)node, 16)));
		item->setText(m_headerToIndex[Name_Class], QString::fromStdString(node->metaObject()->className()));
		item->setText(m_headerToIndex[Name_ObjectName], QString("%0 :(%1)").arg(node->objectName()).arg(node->windowTitle()));

		QRect curRect = node->rect();
		QString rectShow = QString("(%0,%1)(%2,%3) - w:h(%4,%5)")
			.arg(node->mapToGlobal(curRect.topLeft()).x())
			.arg(node->mapToGlobal(curRect.topLeft()).y())
			.arg(node->mapToGlobal(curRect.bottomRight()).x())
			.arg(node->mapToGlobal(curRect.bottomRight()).y())
			.arg(node->width()).arg(node->height());
		item->setText(m_headerToIndex[Name_Rect], rectShow);

		item->setText(m_headerToIndex[Name_Visible], QString("%0").arg(node->isVisible() ? "true" : "false"));
		item->setText(m_headerToIndex[Name_Attribute], WidgetHelper::getWidgetAttributeString(node));
		item->setText(m_headerToIndex[Name_WindowFlags], QString("0x%1").arg(QString::number(node->windowFlags(), 16)));

		QString sytleSrc = node->styleSheet();
		QString sytlenew = sytleSrc.remove(QRegularExpression("\r|\n|\\s"));
		item->setText(m_headerToIndex[Name_StyleSheet], sytlenew);
	}

	void TreeInfoWidgetHelper::showWidgetInfoDetialed(QWidget* widget)
	{
// 		if (!m_widgetInfoModifier)
// 		{
// 			m_widgetInfoModifier = new WidgetInfoModifier(this);
// 			connect(m_widgetInfoModifier, &WidgetInfoModifier::sigUpdateWidget, this, [=](QWidget* widget) {
// 				setItemInfo(m_widgetToTreeItem[widget], widget);
// 				});
// 		}
// 
// 		m_widgetInfoModifier->setTargetWidget(widget);
// 		m_widgetInfoModifier->show();
	}
}