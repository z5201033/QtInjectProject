#include "widgethelper.h"

#include <QApplication>
#include <QClipboard>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QPushButton>
#include <QQueue>
#include <QTreeWidget>
#include <QTextEdit>
#include <QScreen>
#include <QWindow>

#ifdef Q_OS_WIN
#include <windows.h>
#include <wingdi.h>
#endif

#define DLG_WIDTH			400
#define DLG_HEIGHT			500

#define EDITWIDGET_WIDTH	250

#define Name_PickBtn		"pickBtnObject"
#define Name_Addr			"addrObject"
#define Name_Class			"classObject"
#define Name_ObjectName		"objectNameObject"
#define Name_Rect			"rectObject"
#define Name_Width			"widthObject"
#define Name_Height			"heightObject"
#define Name_Visible		"visibleObject"
#define Name_Attribute		"attributeObject"
#define Name_WindowFlags	"windowFlagsObject"
#define Name_StyleSheet		"stylesheetObject"
#define Name_Height			"heightObject"
#define Name_ParentWidget	"parentWidgetObject"
#define Name_ParentAddr		"parentAddrObject"
#define Name_ListCount		"listCount"			// 用于查询列表个数

static int HelperExptionFilter(unsigned int code, struct _EXCEPTION_POINTERS* ep)
{
	Q_UNUSED(code);
	Q_UNUSED(ep);
	return EXCEPTION_EXECUTE_HANDLER;
}

WidgetHelper::WidgetHelper(QWidget* parent)
	: QDialog(parent)
{
	setWindowFlags(Qt::Dialog);
	setAttribute(Qt::WA_ShowModal, false);
	setFixedSize(QSize(DLG_WIDTH, DLG_HEIGHT));

	m_checkWidgetTimer = new QTimer(this);
	m_checkWidgetTimer->setSingleShot(false);
	m_checkWidgetTimer->setInterval(100);

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	QGridLayout *gridLayout = new QGridLayout();

	QPushButton *pickerBtn = new QPushButton(this);
	pickerBtn->setObjectName(Name_PickBtn);
	pickerBtn->setAutoRepeat(true);
	pickerBtn->setFixedSize(QSize(16, 16));
	pickerBtn->setFocusPolicy(Qt::NoFocus);
	int rowCount = gridLayout->rowCount();
	mainLayout->addWidget(pickerBtn, rowCount);

	createItem("Address", Name_Addr, gridLayout);
	createItem("Class Name", Name_Class, gridLayout);
	createItem("Object Name", Name_ObjectName, gridLayout);
	createItem("Rect", Name_Rect, gridLayout);
	createItem("Width", Name_Width, gridLayout);
	createItem("Height", Name_Height, gridLayout);
	createItem("Visible", Name_Visible, gridLayout);
	createItem("Attributes", Name_Attribute, gridLayout);
	createItem("Window Flags", Name_WindowFlags, gridLayout);
	createItem("Style Sheet", Name_StyleSheet, gridLayout);
	createItem("Parent Address", Name_ParentAddr, gridLayout);
	createItem("Parent Class Name", Name_ParentWidget, gridLayout);
	createShowHideBtn("set visible", "", gridLayout);
	
	mainLayout->addLayout(gridLayout);
	mainLayout->addStretch();
	createHighLightBtn(mainLayout);
	createSearchBtn(mainLayout);
	createTreeInfoBtn(mainLayout);

	connect(pickerBtn, &QPushButton::pressed, this, [=]() {

		if (!m_checkWidgetTimer)
			return;

		m_checkWidgetTimer->stop();
		m_checkWidgetTimer->start();
		setCursor(Qt::CrossCursor);
	});

	/*connect(pickerBtn, &QPushButton::released, this, [=]() {

		if (!m_checkWidgetTimer)
			return;

		m_checkWidgetTimer->stop();
	});*/

	connect(m_checkWidgetTimer, &QTimer::timeout, this, &WidgetHelper::onCheckTimeout);

	createCaptureDlg();
}

WidgetHelper::~WidgetHelper()
{

}

bool WidgetHelper::createItem(const QString &caption, const QString &name, QGridLayout *mainLayout)
{
	QLabel *captionLabel = new QLabel(caption, this);
	QLineEdit *lineEdit = new QLineEdit(this);
	lineEdit->setObjectName(name);

	int rowCount = mainLayout->rowCount();
	mainLayout->addWidget(captionLabel, rowCount, 0);
	mainLayout->addWidget(lineEdit, rowCount, 1);
	return true;
}

bool WidgetHelper::createShowHideBtn(const QString &caption, const QString &name, QGridLayout *mainLayout)
{
    Q_UNUSED(name);
	QLabel *captionLabel = new QLabel(caption, this);

	//QWidget *btnContainter = new QWidget(this);
	QHBoxLayout *btnLayout = new QHBoxLayout();
	btnLayout->setContentsMargins(0, 0, 0, 0);
	btnLayout->setSpacing(30);

	QPushButton *showBtn = new QPushButton("show", this);
	QPushButton *hideBtn = new QPushButton("hide", this);
	btnLayout->addWidget(showBtn);
	btnLayout->addWidget(hideBtn);
	btnLayout->addStretch();

	int rowCount = mainLayout->rowCount();
	mainLayout->addWidget(captionLabel, rowCount, 0);
	mainLayout->addLayout(btnLayout, rowCount, 1);

	connect(showBtn, &QPushButton::clicked, this, [=]() {
	
		if (m_curWidget)
		{
			m_curWidget->show();
		}
	});

	connect(hideBtn, &QPushButton::clicked, this, [=]() {

		if (m_curWidget)
		{
			m_curWidget->hide();
		}
	});
	return true;
}

bool WidgetHelper::createHighLightBtn(QVBoxLayout *mainLayout)
{
	QPushButton *highLightBtn = new QPushButton("High light", this);
	mainLayout->addWidget(highLightBtn);

	connect(highLightBtn, &QPushButton::clicked, this, [=]() {

		// 地址
		QLineEdit *lineEdit = findChild<QLineEdit *>(Name_Addr);
		if (!lineEdit)
		{
			return;
		}

		QString addrString = lineEdit->text();
		if (addrString.isEmpty())
		{
			return;
		}

		bool ret = false;
		quint64 addr = addrString.toULongLong(&ret, 16);
		if (!ret)
		{
			return;
		}

		QWidget *curWidget = convertAddrToWidget(addr);
		if (!curWidget)
		{
			clearWidgetData();
			return;
		}

		onHighLightWidget(curWidget);
	});

	return true;
}

bool WidgetHelper::createSearchBtn(QVBoxLayout *mainLayout)
{
	QPushButton *searchBtn = new QPushButton("search", this);

	mainLayout->addWidget(searchBtn);

	connect(searchBtn, &QPushButton::clicked, this, [=]() {

		// 地址
		QLineEdit *lineEdit = findChild<QLineEdit *>(Name_Addr);
		if (!lineEdit)
		{
			clearWidgetData();
			return;
		}

		QString addrString = lineEdit->text();
		if (addrString.isEmpty())
		{
			clearWidgetData();
			return;
		}

		bool ret = false;
		quint64 addr = addrString.toULongLong(&ret, 16);
		if (!ret)
		{
			clearWidgetData();
			return;
		}

		QWidget *curWidget = convertAddrToWidget(addr);
		if (!curWidget)
		{
			clearWidgetData();
			return;
		}

		m_curWidget = curWidget;

		updateWidgetInfo(curWidget);
	});

	return true;
}

bool WidgetHelper::createTreeInfoBtn(QVBoxLayout* mainLayout)
{
	QPushButton* treeInfoBtn = new QPushButton("ShowTreeInfo", this);
	mainLayout->addWidget(treeInfoBtn);

	connect(treeInfoBtn, &QPushButton::clicked, this, [=]() {

		if (!m_treeInfoWidget)
		{
			m_treeInfoWidget = new TreeInfoWidgetHelper(parentWidget());
			connect(m_treeInfoWidget, &TreeInfoWidgetHelper::sigHighLightWidget, this, &WidgetHelper::onHighLightWidget);
		}

		m_treeInfoWidget->setWidgetRootNode(m_curWidget);
		m_treeInfoWidget->show();
	});

	return true;
}

void WidgetHelper::reset()
{
	if (!m_checkWidgetTimer)
		return;

	m_checkWidgetTimer->stop();
	m_preWidget = nullptr;
	setCursor(Qt::ArrowCursor);
	if (m_CaptureWindow)
		m_CaptureWindow->setVisible(false);
}

void WidgetHelper::clearWidgetData()
{
	// 地址
	QLineEdit *lineEdit = findChild<QLineEdit *>(Name_Addr);
	if (lineEdit)
	{
		lineEdit->setText("");
	}

	// 类名
	lineEdit = findChild<QLineEdit *>(Name_Class);
	if (lineEdit)
	{
		lineEdit->setText("");
	}

	// 对象名
	lineEdit = findChild<QLineEdit *>(Name_ObjectName);
	if (lineEdit)
	{
		lineEdit->setText("");
	}

	// 位置
	lineEdit = findChild<QLineEdit *>(Name_Rect);
	if (lineEdit)
	{
		lineEdit->setText("");
	}

	// 窗口宽度
	lineEdit = findChild<QLineEdit *>(Name_Width);
	if (lineEdit)
	{
		lineEdit->setText("");
	}

	// 窗口高度
	lineEdit = findChild<QLineEdit *>(Name_Height);
	if (lineEdit)
	{
		lineEdit->setText("");
	}

	// 窗口可视
	lineEdit = findChild<QLineEdit *>(Name_Visible);
	if (lineEdit)
	{
		lineEdit->setText("");
	}

	// 窗口属性
	lineEdit = findChild<QLineEdit *>(Name_Attribute);
	if (lineEdit)
	{
		lineEdit->setText("");
	}

	// 窗口标志位
	lineEdit = findChild<QLineEdit *>(Name_WindowFlags);
	if (lineEdit)
	{
		lineEdit->setText("");
	}

	// 窗口样式
	lineEdit = findChild<QLineEdit *>(Name_StyleSheet);
	if (lineEdit)
	{
		lineEdit->setText("");
	}

	// 父窗口信息
	lineEdit = findChild<QLineEdit *>(Name_ParentWidget);
	if (lineEdit)
	{
		lineEdit->setText("");
	}

	// 父对象信息
	lineEdit = findChild<QLineEdit *>(Name_ParentAddr);
	if (lineEdit)
	{
		lineEdit->setText("");
	}
}

void WidgetHelper::updateWidgetInfo(QWidget *curWidget)
{
	// 地址
	QLineEdit *lineEdit = findChild<QLineEdit *>(Name_Addr);
	if (!lineEdit)
		return;
	
	lineEdit->setText(QString("0x%0").arg(QString::number((quint64)curWidget, 16)));
	lineEdit->setCursorPosition(0);

	// 类名
	lineEdit = findChild<QLineEdit *>(Name_Class);
	if (!lineEdit)
		return;

	lineEdit->setText(QString::fromStdString(curWidget->metaObject()->className()));
	lineEdit->setCursorPosition(0);

	// 对象名
	lineEdit = findChild<QLineEdit *>(Name_ObjectName);
	if (!lineEdit)
		return;
	lineEdit->setText(curWidget->objectName());
	lineEdit->setCursorPosition(0);

	// 位置
	lineEdit = findChild<QLineEdit *>(Name_Rect);
	if (!lineEdit)
		return;

	QRect curRect = curWidget->rect();
	lineEdit->setText(QString("(%0,%1)(%2,%3)").arg(curWidget->mapToGlobal(curRect.topLeft()).x())
		.arg(curWidget->mapToGlobal(curRect.topLeft()).y())
		.arg(curWidget->mapToGlobal(curRect.bottomRight()).x())
		.arg(curWidget->mapToGlobal(curRect.bottomRight()).y()));
	lineEdit->setCursorPosition(0);

	// 窗口宽度
	lineEdit = findChild<QLineEdit *>(Name_Width);
	if (!lineEdit)
		return;

	lineEdit->setText(QString("%0").arg(curRect.width()));
	lineEdit->setCursorPosition(0);

	// 窗口高度
	lineEdit = findChild<QLineEdit *>(Name_Height);
	if (!lineEdit)
		return;

	lineEdit->setText(QString("%0").arg(curRect.height()));
	lineEdit->setCursorPosition(0);

	// 窗口可视
	lineEdit = findChild<QLineEdit *>(Name_Visible);
	if (!lineEdit)
		return;

	lineEdit->setText(QString("%0").arg(curWidget->isVisible() ? "true" : "false"));
	lineEdit->setCursorPosition(0);

	// 窗口属性
	lineEdit = findChild<QLineEdit *>(Name_Attribute);
	if (!lineEdit)
		return;
	lineEdit->setText(getWidgetAttributeString(curWidget));
	lineEdit->setCursorPosition(0);

	// 窗口标志信息
	lineEdit = findChild<QLineEdit *>(Name_WindowFlags);
	if (!lineEdit)
		return;
	lineEdit->setText(QString("0x%1").arg(QString::number(curWidget->windowFlags(), 16)));
	lineEdit->setCursorPosition(0);

	// 窗口样式信息
	lineEdit = findChild<QLineEdit *>(Name_StyleSheet);
	if (!lineEdit)
		return;
	lineEdit->setText(curWidget->styleSheet());
	lineEdit->setCursorPosition(0);

	// 父窗口信息
	lineEdit = findChild<QLineEdit *>(Name_ParentWidget);
	if (!lineEdit)
		return;

	QWidget *parent = curWidget->parentWidget();
	if (parent)
	{
		lineEdit->setText(QString("%0")
			.arg(parent->metaObject()->className()));
		lineEdit->setCursorPosition(0);
	}

	// 父对象信息
	lineEdit = findChild<QLineEdit *>(Name_ParentAddr);
	if (!lineEdit)
		return;

	QObject *parentObject = curWidget->parent();
	if (parentObject)
	{
		lineEdit->setText(QString("0x%1").arg(QString::number((quint64)parentObject, 16)));
		lineEdit->setCursorPosition(0);
	}
}

QWidget *WidgetHelper::convertAddrToWidget(quint64 addr)
{
#ifdef Q_OS_WIN
	__try
	{
		QWidget *curWidget = dynamic_cast<QWidget *>((QObject *)addr);
		if (!curWidget)
		{
			return nullptr;
		}

		curWidget;
		return curWidget;
	}
	__except (HelperExptionFilter(GetExceptionCode(), GetExceptionInformation()))
	{
		return nullptr;
	}
#else
    return nullptr;
#endif
}

QString WidgetHelper::getWidgetAttributeString(QWidget *curWidget)
{
	QString ret;
	if (!curWidget)
		return ret;

	const int maxAttributeIndex = (int)Qt::WA_AttributeCount;
	for(int index = 0; index < maxAttributeIndex; ++index)
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

void WidgetHelper::updateCaptureDlgPos(QWidget *widget)
{
	if (!widget)
		return;
	
	QRect rect = widget->frameGeometry();
 	rect.moveTopLeft(widget->mapToGlobal(QPoint(0, 0)));
	if (m_CaptureWindow)
	{
		m_CaptureWindow->setGeometry(rect);
		m_CaptureWindow->showNormal();
	}
}

void WidgetHelper::hideCaptureDlg()
{
	if (m_CaptureWindow)
		m_CaptureWindow->hide();
}

#ifdef Q_OS_WIN
bool WidgetHelper::createCaptureDlg()
{
	WNDCLASSEX wcx;

	// Fill in the window class structure with parameters 
	// that describe the main window. 

	wcx.cbSize = sizeof(wcx);          // size of structure 
	wcx.style = CS_HREDRAW |
		CS_VREDRAW;                    // redraw if size changes 
	wcx.lpfnWndProc = wndProc;     // points to window procedure 
	wcx.cbClsExtra = 0;                // no extra class memory 
	wcx.cbWndExtra = 0;                // no extra window memory 
	wcx.hInstance = ::GetModuleHandle(NULL);         // handle to instance 
	wcx.hIcon = LoadIcon(NULL,
		IDI_APPLICATION);              // predefined app. icon 
	wcx.hCursor = LoadCursor(NULL,
		IDC_ARROW);                    // predefined arrow 
	wcx.hbrBackground = nullptr;                  // white background brush 
	wcx.lpszMenuName = TEXT("MainMenu");    // name of menu resource 
	wcx.lpszClassName = TEXT("MainWClass");  // name of window class 
	wcx.hIconSm = NULL;

	// Register the window class. 
	RegisterClassEx(&wcx);

	m_hWnd = ::CreateWindowW(
		L"MainWClass",        // name of window class 
		L"Sample",            // title-bar string 
		WS_POPUP, // top-level window 
		0,       // default horizontal position 
		0,       // default vertical position 
		100,       // default width 
		100,       // default height 
		(HWND)::GetDesktopWindow(),         // no owner window 
		(HMENU)NULL,        // use class menu 
		::GetModuleHandle(NULL),           // handle to application instance 
		(LPVOID)NULL);      // no window-creation data 

	LONG_PTR lStyleEx = ::GetWindowLongPtr(m_hWnd, GWL_EXSTYLE);
	lStyleEx |= WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW;
	::SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, lStyleEx);

	::SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, GetWindowLongPtr(m_hWnd, GWL_EXSTYLE) ^ 0x80000);
	::SetLayeredWindowAttributes(m_hWnd, RGB(255, 255, 255), 255 / 2, LWA_COLORKEY);
	::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	::ShowWindow(m_hWnd, SW_HIDE);
	::UpdateWindow(m_hWnd);

	m_CaptureWindow = QWindow::fromWinId((WId)m_hWnd);

	return true;
}

LRESULT WidgetHelper::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		RECT rc = {};
		::GetClientRect(hwnd, &rc);
		int nWidth = rc.right;
		int nHeight = rc.bottom;;
		HDC hDC;
		PAINTSTRUCT ps;
		hDC = ::BeginPaint(hwnd, &ps);
		HDC hMemDC = ::CreateCompatibleDC(NULL);//建立内存兼容DC
		HBITMAP hBmpMem = ::CreateCompatibleBitmap(hDC, nWidth, nHeight);
		::SelectObject(hMemDC, hBmpMem);
		::SelectObject(hMemDC, ::GetStockObject(WHITE_BRUSH));
		HPEN hPen = ::CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
		::SelectObject(hMemDC, hPen);

		::Rectangle(hMemDC, 0, 0, rc.right, rc.bottom);
		::BitBlt(hDC, 0, 0, nWidth, nHeight, hMemDC, 0, 0, SRCCOPY);//将内存DC中的内容拷贝到当前窗口DC中,并显示


		::DeleteObject(hBmpMem);//删除对象
		::DeleteObject(hPen);//删除对象
		::DeleteDC(hMemDC);//删除DC；

		::EndPaint(hwnd, &ps);
	}
	break;
	case WM_ERASEBKGND:
	{
		return true;
	}
	break;
	default:
		break;
	}

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

#endif

void WidgetHelper::onCheckTimeout()
{
	QWidget *curWidget = QApplication::widgetAt(QCursor::pos());
	m_curWidget = curWidget;
	if (m_preWidget == curWidget)
	{
		if (!(QApplication::mouseButtons() & Qt::LeftButton) && m_checkWidgetTimer)
		{
			reset();
		}
		return;
	}

	if (!curWidget || (curWidget && ((curWidget == this) || curWidget->parentWidget() == this)))
	{
		clearWidgetData();
		if (m_CaptureWindow)
			m_CaptureWindow->setVisible(false);

		m_preWidget = nullptr;
		return;
	};

	m_preWidget = curWidget;
	updateCaptureDlgPos(curWidget);
	
	updateWidgetInfo(curWidget);

	if (!(QApplication::mouseButtons() & Qt::LeftButton) && m_checkWidgetTimer)
	{
		reset();
	}
}

void WidgetHelper::onHighLightWidget(QWidget* widget)
{
	if (!widget)
		return;

	if (!m_highLightTimer)
	{
		m_highLightTimer = new QTimer(this);
		m_highLightTimer->setInterval(100);
		connect(m_highLightTimer, &QTimer::timeout, this, [=]() {

			if (m_highLightCount % 2 == 0)
			{
				updateCaptureDlgPos(m_highLightWidget);
			}
			else
			{
				hideCaptureDlg();
			}

			if (++m_highLightCount > 10)
			{
				m_highLightCount = 0;
				m_highLightWidget = nullptr;
				m_highLightTimer->stop();
				hideCaptureDlg();
			}
		});
	}

	m_highLightWidget = widget;
	m_highLightCount = 0;
	m_highLightTimer->start();
}

//////////////////////////////////////////////////////////////////////////
// TreeInfoWidget
TreeInfoWidgetHelper::TreeInfoWidgetHelper(QWidget* parent)
	: QDialog(parent)
{
	resize(1080, 800);

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

		showWidgetInfoModifier(m_treeItemToWidget[item]);
	});

	connect(m_treeInfo, &QTreeWidget::itemPressed, this, [=](QTreeWidgetItem* item, int column) {
		if (!item)
			return;

		if (qApp->mouseButtons() != Qt::RightButton)
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

void TreeInfoWidgetHelper::setWidgetRootNode(QWidget* root)
{
	m_treeInfo->clear();
	m_treeItemToWidget.clear();
	m_widgetToTreeItem.clear();
	if (!root)
		return;

	QTreeWidgetItem* itemTop = new QTreeWidgetItem();
	setItemInfo(itemTop, root);
	m_treeItemToWidget[itemTop] = root;
	m_widgetToTreeItem[root] = itemTop;

	m_treeInfo->addTopLevelItem(itemTop);


	addChildInfo(itemTop, root);
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

		if (itemWidget.first->childCount() > 0 && itemDepth(itemWidget.first) < 6)
			itemWidget.first->setExpanded(true);
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

void TreeInfoWidgetHelper::showWidgetInfoModifier(QWidget* widget)
{
	if (!m_widgetInfoModifier)
	{
		m_widgetInfoModifier = new WidgetInfoModifier(this);
		connect(m_widgetInfoModifier, &WidgetInfoModifier::sigUpdateWidget, this, [=](QWidget* widget) {
			setItemInfo(m_widgetToTreeItem[widget], widget);
		});
	}

	m_widgetInfoModifier->setTargetWidget(widget);
	m_widgetInfoModifier->show();
}

//////////////////////////////////////////////////////////////////////////
// WidgetInfoModifier
WidgetInfoModifier::WidgetInfoModifier(QWidget* parent/* = nullptr*/)
	: QDialog(parent)
{
	resize(500, 600);
	initUI();
}

WidgetInfoModifier::~WidgetInfoModifier()
{
}

void WidgetInfoModifier::setTargetWidget(QWidget* widget)
{
	m_targetWidget = widget;
	updateAllWidgetInfo();
}

void WidgetInfoModifier::initUI()
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setSpacing(8);
	mainLayout->setContentsMargins(8, 8, 8, 8);

	m_tabWidget = new QTabWidget(this);
	m_tabWidget->setContentsMargins(0, 0, 0, 0);
// 	m_tabWidget->setStyleSheet("QTabWidget#tabWidget{background:transparent;}\
// 								QTabWidget::pane{border:0px;}\
//                                  QTabBar::tab{background-color:#1F1F1F;color:#999999;}\
//                                  QTabBar::tab::selected{background-color:#0F0F0F;color:#EEEEEE;}");
	m_tabWidget->addTab(addBaseInfoWidget(), "BaseInfo");
	m_tabWidget->addTab(addStyleSheetWidget(), "StyleSheet");

	QHBoxLayout* hlayout = new QHBoxLayout(this);
	{
		QPushButton* btnApply = new QPushButton(this);
		btnApply->setFixedSize(70, 20);
		btnApply->setText("Apply All");
		QPushButton* btnRefresh = new QPushButton(this);
		btnRefresh->setFixedSize(70, 20);
		btnRefresh->setText("Refresh");
		hlayout->addStretch();
		hlayout->addWidget(btnApply);
		hlayout->addWidget(btnRefresh);

		connect(btnApply, &QPushButton::clicked, this, [=]() {
			applyAllWidgetInfo();
		});
		connect(btnRefresh, &QPushButton::clicked, this, [=]() {
			updateAllWidgetInfo();
		});
	}

	mainLayout->addWidget(m_tabWidget);
	mainLayout->addLayout(hlayout);
}

QWidget* WidgetInfoModifier::addBaseInfoWidget()
{
	QWidget* widget = new QWidget(this);
	widget->setObjectName("BaseInfoWidget");
	widget->setStyleSheet("QWidget#BaseInfoWidget{background:transparent;}");
	QVBoxLayout* layout = new QVBoxLayout(widget);

	return widget;
}

QWidget* WidgetInfoModifier::addStyleSheetWidget()
{
	QWidget* widget = new QWidget(this);
	widget->setObjectName("StyleSheetWidget");
	widget->setStyleSheet("QWidget#StyleSheetWidget{background:transparent;}");
	QVBoxLayout* layout = new QVBoxLayout(widget);

	QTextEdit* textEdit = new QTextEdit(widget);
	textEdit->setObjectName("StyleSheetEdit");

	layout->addWidget(textEdit);

	return widget;
}

void WidgetInfoModifier::applyAllWidgetInfo()
{
	QTextEdit* textEdit = findChild<QTextEdit*>("StyleSheetEdit");
	if (textEdit)
	{
		QString text = textEdit->toPlainText();
		m_targetWidget->setStyleSheet(text);
	}

	emit sigUpdateWidget(m_targetWidget);
}

void WidgetInfoModifier::updateAllWidgetInfo()
{
	QTextEdit* textEdit = findChild<QTextEdit*>("StyleSheetEdit");
	if (textEdit)
	{
		QString text = m_targetWidget ? m_targetWidget->styleSheet() : "";
		textEdit->setPlainText(text);
	}
}