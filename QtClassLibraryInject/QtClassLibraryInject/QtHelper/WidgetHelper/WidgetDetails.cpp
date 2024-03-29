#include "WidgetDetails.h"
#include "CustomListItem.h"
#include "../CaptureDlg.h"
#include "../Common.h"
#include "../../Utils/QtEnumUtils.h"

#include <QApplication>
#include <QClipboard>
#include <QCheckBox>
#include <QDebug>
#include <QMetaEnum>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QMap>

namespace Qth
{
	struct WidgetDetailsrivate
	{
		QTabWidget* m_tabWidget = nullptr;
		std::map<QWidget*, std::function<void(bool, bool)>> m_applyFuncList;

		// BaseInfo
		QLineEdit* m_biEditaddr = nullptr;
		QLineEdit* m_biEditClass = nullptr;
		QLineEdit* m_biEditObjName = nullptr;
		QLineEdit* m_biEditTitle = nullptr;
		QLineEdit* m_biEditRectX = nullptr;
		QLineEdit* m_biEditRectY = nullptr;
		QLineEdit* m_biEditRectWidth = nullptr;
		QLineEdit* m_biEditRectHeight = nullptr;
		QLineEdit* m_biEditMinWidth = nullptr;
		QLineEdit* m_biEditMaxWidth = nullptr;
		QLineEdit* m_biEditMinHeight = nullptr;
		QLineEdit* m_biEditMaxHeight = nullptr;
		QCheckBox* m_biCheckBoxVisible = nullptr;

		// AttributeInfo
		typedef QMap<Qt::WidgetAttribute, QListWidgetItem*> AttributeItemMap;
		QListWidget*			m_attributeListWidget = nullptr;
		AttributeItemMap		m_attributeItemMap;

		// WindowFlag
		typedef QMap<UINT/*Qt::WindowType*/, QListWidgetItem*> WindowTypeItemMap;
		QListWidget*			m_windowFlagListWidget = nullptr;
		WindowTypeItemMap	m_windowFlagItemMap;

		// StyleSheet
		QTextEdit*	m_styleSheetEdit = nullptr;
		// StyleSheet
		QWidget*		m_dynamicPropertyContainerWidget = nullptr;
		QTextEdit*	m_dynamicPropertyEdit = nullptr;
	};

	static QString getWindowFlagSubItem(const QList<UINT>& list, const QMetaEnum& me)
	{
		if (list.size() < 3)
			return QString();

		UINT curType = list.back();
		QList<UINT> subList;
		for (int i = list.size() - 2; i >= 1; i--)
		{
			if (~curType & list[i])
				continue;

			bool hadContain = false;
			for (int j = 0; j < subList.size(); j++)
			{
				if (~subList[j] & list[i])
					continue;

				hadContain = true;
				break;
			}

			if (!hadContain)
				subList.push_back(list[i]);
		}

		if (subList.size() == 0)
			return QString();

		QString strRet;
		for (int i = 0; i < subList.size(); i++)
		{
			if (i > 0)
				strRet += " | ";
			strRet += QString::fromStdString(me.valueToKey(subList[i]));
			curType &= ~subList[i];
		}

		if (curType)
		{
			strRet.insert(0, QString("0x%0 | ").arg(curType, 8, 16, QLatin1Char('0')));
		}

		return strRet;
	}

	//////////////////////////////////////////////////////////////////////////
	// WidgetDetails
	WidgetDetails::WidgetDetails(QWidget* parent/* = nullptr*/)
		: QDialog(parent)
		, d(new WidgetDetailsrivate)
	{
		setWindowFlag(Qt::WindowMinMaxButtonsHint);
		resize(500, 600);
		initUI();
	}

	WidgetDetails::~WidgetDetails()
	{
	}

	void WidgetDetails::setTargetWidget(QWidget* widget)
	{
		QString titleText = "null";
		if (widget)
			titleText = QString("%0(0x%1)").arg(widget->metaObject()->className()).arg(QString::number((quint64)widget, 16));
		setWindowTitle(titleText);

		m_targetWidget = widget;
		onUpdateAllWidgetInfo();
	}

	void WidgetDetails::initUI()
	{
		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->setSpacing(8);
		mainLayout->setContentsMargins(8, 8, 8, 8);

		d->m_tabWidget = new QTabWidget(this);
		d->m_tabWidget->setContentsMargins(0, 0, 0, 0);
		d->m_tabWidget->setElideMode(Qt::ElideRight);
		d->m_tabWidget->setMovable(true);
		d->m_tabWidget->addTab(addBaseInfoWidget(), "BaseInfo");
		d->m_tabWidget->addTab(addAttributeInfoWidget(), "Attribute");
		d->m_tabWidget->addTab(addWindowFlagWidget(), "WindowFlag");
		d->m_tabWidget->addTab(addStyleSheetWidget(), "StyleSheet");
		d->m_dynamicPropertyContainerWidget = addDynamicPropertyWidget();
		d->m_tabWidget->addTab(d->m_dynamicPropertyContainerWidget, "DynamicProperty");
		
		QHBoxLayout* hlayout = new QHBoxLayout(this);
		{
			QPushButton* btnApply = new QPushButton(this);
			btnApply->setFixedSize(70, 20);
			btnApply->setText("Apply");
			btnApply->setFocusPolicy(Qt::NoFocus);
			QPushButton* btnApplyAll = new QPushButton(this);
			btnApplyAll->setFixedSize(70, 20);
			btnApplyAll->setText("Apply All");
			btnApplyAll->setFocusPolicy(Qt::NoFocus);
			QPushButton* btnRefresh = new QPushButton(this);
			btnRefresh->setFixedSize(70, 20);
			btnRefresh->setText("Refresh");
			btnRefresh->setFocusPolicy(Qt::NoFocus);
			hlayout->addStretch();
			hlayout->addWidget(btnApply);
			hlayout->addWidget(btnApplyAll);
			hlayout->addWidget(btnRefresh);

			connect(btnApply, &QPushButton::clicked, this, &WidgetDetails::onApplyCurrentTab);
			connect(btnApplyAll, &QPushButton::clicked, this, &WidgetDetails::onApplyAllWidgetInfo);
			connect(btnRefresh, &QPushButton::clicked, this, &WidgetDetails::onUpdateAllWidgetInfo);

			connect(d->m_tabWidget, &QTabWidget::currentChanged, [=]() {
				bool isDPWidget = (d->m_dynamicPropertyContainerWidget == d->m_tabWidget->currentWidget());
				btnApply->setEnabled(!isDPWidget);
			});
			void currentChanged(int index);
		}

		mainLayout->addWidget(d->m_tabWidget);
		mainLayout->addLayout(hlayout);
	}

	QWidget* WidgetDetails::addBaseInfoWidget()
	{
		QWidget* baseWidget = new QWidget(this);
		baseWidget->setObjectName("BaseInfoWidget");
		baseWidget->setStyleSheet("QWidget#BaseInfoWidget{background:transparent;}");
		QVBoxLayout* baseLayout = new QVBoxLayout(baseWidget);
		baseLayout->setSpacing(8);
		baseLayout->setContentsMargins(4, 8, 4, 0);

		QGridLayout* gridLayout = new QGridLayout();

		auto createItemFunc = [=](QHBoxLayout* layout, QLineEdit*& edit, const QString& title, bool readOnly = false)->QBoxLayout* {
			if (!layout)
			{
				layout = new QHBoxLayout();
				layout->setSpacing(0);
				layout->setContentsMargins(0, 0, 0, 0);
			}

			QLabel* label = new QLabel(title, baseWidget);
			label->setFixedWidth(Common::getContentWidth(label->font(), title).width() + 8);
			edit = new QLineEdit(baseWidget);
			edit->setReadOnly(readOnly);

			layout->addWidget(label);
			layout->addWidget(edit);
			return layout;
		};

		int row = gridLayout->rowCount();
		gridLayout->addLayout(createItemFunc(nullptr, d->m_biEditClass, "ClassName:", true), row, 0);
		gridLayout->addLayout(createItemFunc(nullptr, d->m_biEditaddr, "Address:", true), row, 1);
		row = gridLayout->rowCount();
		gridLayout->addLayout(createItemFunc(nullptr, d->m_biEditObjName, "ObjectName:"), row, 0);
		gridLayout->addLayout(createItemFunc(nullptr, d->m_biEditTitle, "Title:"), row, 1);

		row = gridLayout->rowCount();
		QHBoxLayout* hLayout = new QHBoxLayout();
		{
			hLayout->setSpacing(0);
			hLayout->setContentsMargins(0, 0, 0, 0);
			createItemFunc(hLayout, d->m_biEditRectX, "TopLeftX:");
			hLayout->addSpacing(8);
			createItemFunc(hLayout, d->m_biEditRectY, "TopLeftY:");
			d->m_biEditRectX->setValidator(new QIntValidator(-655360, 655360, d->m_biEditRectX));
			d->m_biEditRectY->setValidator(new QIntValidator(-655360, 655360, d->m_biEditRectY));
		}
		gridLayout->addLayout(hLayout, row, 0);
		hLayout = new QHBoxLayout();
		{
			hLayout->setSpacing(0);
			hLayout->setContentsMargins(0, 0, 0, 0);
			createItemFunc(hLayout, d->m_biEditRectWidth, "Width:");
			hLayout->addSpacing(8);
			createItemFunc(hLayout, d->m_biEditRectHeight, "Height:");
			d->m_biEditRectWidth->setValidator(new QIntValidator(0, 655360, d->m_biEditRectWidth));
			d->m_biEditRectHeight->setValidator(new QIntValidator(0, 655360, d->m_biEditRectHeight));
		}
		gridLayout->addLayout(hLayout, row, 1);

		row = gridLayout->rowCount();
		hLayout = new QHBoxLayout();
		{
			hLayout->setSpacing(0);
			hLayout->setContentsMargins(0, 0, 0, 0);
			createItemFunc(hLayout, d->m_biEditMinWidth, "MinWidth:");
			hLayout->addSpacing(8);
			createItemFunc(hLayout, d->m_biEditMaxWidth, "MaxWidth:");
			d->m_biEditMinWidth->setValidator(new QIntValidator(-655360, 655360, d->m_biEditMinWidth));
			d->m_biEditMaxWidth->setValidator(new QIntValidator(-655360, 655360, d->m_biEditMaxWidth));
		}
		gridLayout->addLayout(hLayout, row, 0);
		hLayout = new QHBoxLayout();
		{
			hLayout->setSpacing(0);
			hLayout->setContentsMargins(0, 0, 0, 0);
			createItemFunc(hLayout, d->m_biEditMinHeight, "MinHeight:");
			hLayout->addSpacing(8);
			createItemFunc(hLayout, d->m_biEditMaxHeight, "MaxHeight:");
			d->m_biEditMinHeight->setValidator(new QIntValidator(0, 655360, d->m_biEditMinHeight));
			d->m_biEditMaxHeight->setValidator(new QIntValidator(0, 655360, d->m_biEditMaxHeight));
		}
		gridLayout->addLayout(hLayout, row, 1);

		row = gridLayout->rowCount();
		d->m_biCheckBoxVisible = new QCheckBox(baseWidget);
		d->m_biCheckBoxVisible->setText("visible");
		connect(d->m_biCheckBoxVisible, &QCheckBox::stateChanged, this, &WidgetDetails::onTargetVisibelStateChanged);
		gridLayout->addWidget(d->m_biCheckBoxVisible, row, 0);

		baseLayout->addLayout(gridLayout);
		baseLayout->addStretch();

		d->m_applyFuncList[baseWidget] = std::bind(&WidgetDetails::applyBaseInfo, this, std::placeholders::_1, std::placeholders::_2);
		return baseWidget;
	}

	QWidget* WidgetDetails::addAttributeInfoWidget()
	{
		d->m_attributeListWidget = new QListWidget(this);
		d->m_attributeListWidget->setObjectName("AttributeInfoWidget");
		d->m_attributeListWidget->setSortingEnabled(true);
		d->m_attributeItemMap.clear();

		QMetaEnum me = QtEnumUtils::getMetaEnum(Qt::WA_Disabled);
		for (int i = 0; i < me.keyCount(); ++i)
		{
			Qt::WidgetAttribute type = static_cast<Qt::WidgetAttribute>(me.value(i));
			QString text = QString("%0 %1").arg(me.value(i)).arg(me.key(i));

			QListWidgetItem* item = new AttributeListItem(type, d->m_attributeListWidget);
			item->setCheckState(Qt::Unchecked);
			item->setText(text);
			d->m_attributeListWidget->addItem(item);

			d->m_attributeItemMap[type] = item;
		}

		d->m_applyFuncList[d->m_attributeListWidget] = std::bind(&WidgetDetails::applyAttribute, this, std::placeholders::_1, std::placeholders::_2);
		return d->m_attributeListWidget;
	}

	QWidget* WidgetDetails::addWindowFlagWidget()
	{
		d->m_windowFlagListWidget = new QListWidget(this);
		d->m_windowFlagListWidget->setObjectName("addWindowFlagWidget");
		d->m_windowFlagListWidget->setSortingEnabled(true);
		d->m_windowFlagItemMap.clear();

		QMetaEnum me = QtEnumUtils::getMetaEnum(Qt::Widget);
		QList<UINT> cacheList;
		for (int i = 0; i < me.keyCount(); ++i)
		{
			Qt::WindowType type = static_cast<Qt::WindowType>(me.value(i));
			if (d->m_windowFlagItemMap.count(type) > 0)
				continue;

			UINT curType = (UINT)type;
			QString text = QString("0x%0 %1").arg(curType, 8, 16, QLatin1Char('0')).arg(me.key(i));
			cacheList.push_back(curType);
			QString subItem = getWindowFlagSubItem(cacheList, me);
			if (!subItem.isEmpty())
			{
				text += " = ";
				text += subItem;
			}

			QListWidgetItem* item = new WindowsFlagListItem(type, d->m_windowFlagListWidget);
			item->setCheckState(Qt::Unchecked);
			item->setText(text);
			if (type == Qt::Widget)
			{
				item->setCheckState(Qt::Checked);
				item->setFlags(item->flags() & ~Qt::ItemIsUserCheckable);
			}
			d->m_windowFlagListWidget->addItem(item);
			d->m_windowFlagItemMap[type] = item;
		}

		d->m_applyFuncList[d->m_windowFlagListWidget] = std::bind(&WidgetDetails::applyWindowFlag, this, std::placeholders::_1, std::placeholders::_2);
		return d->m_windowFlagListWidget;
	}

	QWidget* WidgetDetails::addStyleSheetWidget()
	{
		QWidget* widget = new QWidget(this);
		widget->setObjectName("StyleSheetWidget");
		widget->setStyleSheet("QWidget#StyleSheetWidget{background:transparent;}");
		QVBoxLayout* layout = new QVBoxLayout(widget);

		d->m_styleSheetEdit = new QTextEdit(widget);
		layout->addWidget(d->m_styleSheetEdit);

		d->m_applyFuncList[widget] = std::bind(&WidgetDetails::applyStyleSheet, this, std::placeholders::_1, std::placeholders::_2);
		return widget;
	}

	QWidget* WidgetDetails::addDynamicPropertyWidget()
	{
		QWidget* widget = new QWidget(this);
		widget->setObjectName("DynamicPropertyWidget");
		widget->setStyleSheet("QWidget#DynamicPropertyWidget{background:transparent;}");
		QVBoxLayout* layout = new QVBoxLayout(widget);

		d->m_dynamicPropertyEdit = new QTextEdit(widget);
		d->m_dynamicPropertyEdit->setReadOnly(true);
		layout->addWidget(d->m_dynamicPropertyEdit);

		return widget;
	}

	void WidgetDetails::onApplyAllWidgetInfo()
	{
		if (!checkTargetWidgetValid(true))
			return;

		for (const auto& it : d->m_applyFuncList)
		{
			it.second(false, false);
		}

		emit sigNeedUpdateWidget(m_targetWidget);
		onUpdateAllWidgetInfo();
	}

	void WidgetDetails::onUpdateAllWidgetInfo()
	{
		updateBaseInfo();
		updateStyleSheet();
		updateAttribute();
		updateWindowFlag();
		updateDynamicProperty();
	}

	void WidgetDetails::applyBaseInfo(bool sendSignal, bool showErrorMsg)
	{
		if (!checkTargetWidgetValid(showErrorMsg))
			return;

		auto applyDataFunc = [=](QLineEdit* edit, std::function<QString()> getFunc, std::function<void(const QString&)> setFunc) {
			if (edit) {
				QString objectNameOld = getFunc();
				QString objectNameNew = edit->text();
				if (objectNameOld != objectNameNew)
					setFunc(objectNameNew);
			}
		};
		applyDataFunc(d->m_biEditObjName, std::bind(&QWidget::objectName, m_targetWidget), 
			std::bind(&QWidget::setObjectName, m_targetWidget, std::placeholders::_1));
		applyDataFunc(d->m_biEditTitle, std::bind(&QWidget::windowTitle, m_targetWidget),
			std::bind(&QWidget::setWindowTitle, m_targetWidget, std::placeholders::_1));

		// Rect
		do 
		{
			if (!d->m_biEditRectX || !d->m_biEditRectY || !d->m_biEditRectWidth || !d->m_biEditRectHeight)
				break;

			QRect frameRectOld = m_targetWidget->frameGeometry();
			QRect clientRectOld = m_targetWidget->rect();
			QPoint posTopLeftOld = m_targetWidget->mapToGlobal(QPoint(clientRectOld.width() - frameRectOld.width(), clientRectOld.height() - frameRectOld.height()));
			frameRectOld.moveTo(posTopLeftOld);

			QRect frameRectNew;
			frameRectNew.setX(d->m_biEditRectX->text().toInt());
			frameRectNew.setY(d->m_biEditRectY->text().toInt());
			frameRectNew.setWidth(d->m_biEditRectWidth->text().toInt());
			frameRectNew.setHeight(d->m_biEditRectHeight->text().toInt());

			if (frameRectOld == frameRectNew)
				break;

			if (!m_targetWidget->isWindow())
			{
				QWidget* parent = m_targetWidget->parentWidget();
				if (!parent)
					break;
				QPoint posNew = parent->mapFromGlobal(frameRectNew.topLeft());
				frameRectNew.moveTo(posNew);
			}

			m_targetWidget->resize(frameRectNew.size());
			m_targetWidget->move(frameRectNew.topLeft());
		} while (false);	

		// fixSize
		bool fixSizeChanged = false;
		auto applyFixSizeFunc = [&](QLineEdit* edit, std::function<int()> getFunc, std::function<void(int)> setFunc) {
			if (edit) {
				int len = edit->text().toInt();
				if (len != getFunc()) {
					setFunc(len);
					fixSizeChanged = true;
				}
			}
		};
		applyFixSizeFunc(d->m_biEditMinWidth, std::bind(&QWidget::minimumWidth, m_targetWidget), std::bind(&QWidget::setMinimumWidth, m_targetWidget, std::placeholders::_1));
		applyFixSizeFunc(d->m_biEditMaxWidth, std::bind(&QWidget::maximumWidth, m_targetWidget), std::bind(&QWidget::setMaximumWidth, m_targetWidget, std::placeholders::_1));
		applyFixSizeFunc(d->m_biEditMinHeight, std::bind(&QWidget::minimumHeight, m_targetWidget), std::bind(&QWidget::setMinimumHeight, m_targetWidget, std::placeholders::_1));
		applyFixSizeFunc(d->m_biEditMaxHeight, std::bind(&QWidget::minimumHeight, m_targetWidget), std::bind(&QWidget::setMaximumHeight, m_targetWidget, std::placeholders::_1));

		if (fixSizeChanged)
			updateBaseInfo(false);
	}

	void WidgetDetails::updateBaseInfo(bool showErrorMsg)
	{
		bool valid = checkTargetWidgetValid(showErrorMsg);

		auto setDataFunc = [=](QLineEdit* edit, const QString& text) {
			if (edit)
				edit->setText(text);
		};

		setDataFunc(d->m_biEditaddr, valid ? QString("0x%0").arg(QString::number((quint64)m_targetWidget.data(), 16)) : "");
		setDataFunc(d->m_biEditClass, valid ? QString::fromStdString(m_targetWidget->metaObject()->className()) : "");
		setDataFunc(d->m_biEditObjName, valid ? m_targetWidget->objectName() : "");
		setDataFunc(d->m_biEditTitle, valid ? m_targetWidget->windowTitle() : "");

		QRect frameRect;
		if (valid)
		{
			frameRect = m_targetWidget->frameGeometry();
			QRect clientRect = m_targetWidget->rect();
			QPoint posTopLeft = m_targetWidget->mapToGlobal(QPoint(clientRect.width() - frameRect.width(), clientRect.height() - frameRect.height()));
			frameRect.moveTo(posTopLeft);
		}
		setDataFunc(d->m_biEditRectX, valid ? QString::number(frameRect.x()) : "");
		setDataFunc(d->m_biEditRectY, valid ? QString::number(frameRect.y()) : "");
		setDataFunc(d->m_biEditRectWidth, valid ? QString::number(frameRect.width()) : "");
		setDataFunc(d->m_biEditRectHeight, valid ? QString::number(frameRect.height()) : "");

		setDataFunc(d->m_biEditMinWidth, valid ? QString::number(m_targetWidget->minimumWidth()) : "");
		setDataFunc(d->m_biEditMaxWidth, valid ? QString::number(m_targetWidget->maximumWidth()) : "");
		setDataFunc(d->m_biEditMinHeight, valid ? QString::number(m_targetWidget->minimumHeight()) : "");
		setDataFunc(d->m_biEditMaxHeight, valid ? QString::number(m_targetWidget->maximumHeight()) : "");

		if (d->m_biCheckBoxVisible)
			d->m_biCheckBoxVisible->setChecked(valid && m_targetWidget->isVisible());
	}

	void WidgetDetails::applyStyleSheet(bool sendSignal, bool showErrorMsg)
	{
		if (!checkTargetWidgetValid(showErrorMsg))
			return;

		if (d->m_styleSheetEdit)
		{
			QString text = d->m_styleSheetEdit->toPlainText();
			m_targetWidget->setStyleSheet(text);
		}

		if (sendSignal)
		{
			emit sigNeedUpdateWidget(m_targetWidget);
			onUpdateAllWidgetInfo();
		}
	}

	void WidgetDetails::updateStyleSheet(bool showErrorMsg)
	{
		bool valid = checkTargetWidgetValid(showErrorMsg);

		if (d->m_styleSheetEdit)
		{
			QString text = valid ? m_targetWidget->styleSheet() : "";
			d->m_styleSheetEdit->setPlainText(text);
		}
	}

	void WidgetDetails::updateDynamicProperty(bool showErrorMsg)
	{
		bool valid = checkTargetWidgetValid(showErrorMsg);

		if (!d->m_dynamicPropertyEdit)
			return;

		if (!valid)
		{
			d->m_dynamicPropertyEdit->setPlainText("");
			return;
		}
		
		QString text;
		QDebug dbg(&text);
		QList<QByteArray> names = m_targetWidget->dynamicPropertyNames();
		for (int i = 0; i < names.size(); i++)
		{
			text += "[";
			text += names[i].data();
			text += "]\n";
			dbg << m_targetWidget->property(names[i].data()) << "\n";
		}

		d->m_dynamicPropertyEdit->setPlainText(text);
	}

	void WidgetDetails::applyAttribute(bool sendSignal, bool showErrorMsg)
	{
		if (!checkTargetWidgetValid(showErrorMsg))
			return;

		auto it = d->m_attributeItemMap.begin();
		for (; it != d->m_attributeItemMap.end(); ++it)
		{
			bool src = m_targetWidget->testAttribute(it.key());
			bool dst = it.value()->checkState() == Qt::Checked;
			if (src != dst)
				m_targetWidget->setAttribute(it.key(), dst);
		}

		if (sendSignal)
		{
			emit sigNeedUpdateWidget(m_targetWidget);
			onUpdateAllWidgetInfo();
		}
	}

	void WidgetDetails::updateAttribute(bool showErrorMsg)
	{
		bool valid = checkTargetWidgetValid(showErrorMsg);

		auto it = d->m_attributeItemMap.begin();
		for (; it != d->m_attributeItemMap.end(); ++it)
		{
			bool src = valid && m_targetWidget->testAttribute(it.key());
			it.value()->setCheckState(src ? Qt::Checked : Qt::Unchecked);
		}
	}

	void WidgetDetails::applyWindowFlag(bool sendSignal, bool showErrorMsg)
	{
		if (!checkTargetWidgetValid(showErrorMsg))
			return;

		Qt::WindowFlags flagsOld = m_targetWidget->windowFlags();
		Qt::WindowFlags flagsNew;
		auto it = d->m_windowFlagItemMap.begin();
		for (; it != d->m_windowFlagItemMap.end(); ++it)
		{
			bool dst = it.value()->checkState() == Qt::Checked;
			if (dst)
				flagsNew |= static_cast<Qt::WindowType>(it.key());
		}

		if (flagsOld == flagsNew)
			return;

		m_targetWidget->setWindowFlags(flagsNew);

		if (sendSignal)
		{
			emit sigNeedUpdateWidget(m_targetWidget);
			onUpdateAllWidgetInfo();
		}
	}

	void WidgetDetails::updateWindowFlag(bool showErrorMsg)
	{
		bool valid = checkTargetWidgetValid(showErrorMsg);

		Qt::WindowFlags flags = valid ? m_targetWidget->windowFlags() : Qt::WindowFlags();
		auto it = d->m_windowFlagItemMap.begin();
		for (; it != d->m_windowFlagItemMap.end(); ++it)
		{
			bool src = it.key() == Qt::Widget || flags.testFlag(static_cast<Qt::WindowType>(it.key()));
			it.value()->setCheckState(src ? Qt::Checked : Qt::Unchecked);
		}
	}

	bool WidgetDetails::checkTargetWidgetValid(bool showErrorMsg)
	{
		if (!m_targetWidget)
		{
			if (showErrorMsg)
				QMessageBox::information(this, "warning", "Current widget is invalid!");
			return false;
		}

		return true;
	}

	void WidgetDetails::onTargetVisibelStateChanged(int state)
	{
		// Visible
		do
		{
			if (!d->m_biCheckBoxVisible)
				break;

			bool visibleOld = m_targetWidget->isVisible();
			bool visibleNew = d->m_biCheckBoxVisible->isChecked();
			if (visibleOld == visibleNew)
				break;

			m_targetWidget->setVisible(visibleNew);

			updateAttribute();
		} while (false);
	}

	void WidgetDetails::onApplyCurrentTab()
	{
		if (!d->m_tabWidget)
			return;

		QWidget* currentWidget = d->m_tabWidget->currentWidget();
		auto find = d->m_applyFuncList.find(currentWidget);
		if (find != d->m_applyFuncList.end())
			find->second(true, true);
	}
}