﻿#include "QthWidgetDetails.h"
#include "QthCaptureDlg.h"
#include "QthCommon.h"

#include <QApplication>
#include <QClipboard>
#include <QCheckBox>
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

class QListWidget;
class QListWidgetItem;
class QTabWidget;

namespace Qth
{
	struct WidgetDetailsrivate
	{
		QTabWidget* m_tabWidget = nullptr;

		// BaseInfo
		QLineEdit* m_biEditaddr = nullptr;
		QLineEdit* m_biEditClass = nullptr;
		QLineEdit* m_biEditObjName = nullptr;
		QLineEdit* m_biEditTitle = nullptr;
		QLineEdit* m_biEditRectX = nullptr;
		QLineEdit* m_biEditRectY = nullptr;
		QLineEdit* m_biEditRectWidth = nullptr;
		QLineEdit* m_biEditRectHeight = nullptr;
		QCheckBox* m_biCheckBoxVisible = nullptr;

		// AttributeInfo
		typedef QMap<Qt::WidgetAttribute, QListWidgetItem*> AttributeItemMap;
		QListWidget*			m_attributeListWidget = nullptr;
		AttributeItemMap		m_attributeItemMap;

		// StyleSheet
		QTextEdit*			m_styleSheetEdit = nullptr;
	};

	class AttributeListItem : public QListWidgetItem
	{
	public:
		const static int s_arributeRole = Qt::UserRole;

		explicit AttributeListItem(Qt::WidgetAttribute attri, QListWidget* listview = nullptr)
			: QListWidgetItem(listview)
		{
			setData(s_arributeRole, (int)attri);
		}

		bool operator<(const QListWidgetItem& other) const override
		{
			bool curCheck = checkState() == Qt::Checked;
			bool otherCheck = other.checkState() == Qt::Checked;
			if (curCheck != otherCheck)
				return curCheck;

			int curAttri = data(s_arributeRole).toInt();
			int otherAttri = other.data(s_arributeRole).toInt();

			return curAttri < otherAttri;
		}
	};

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
		updateAllWidgetInfo();
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
		d->m_tabWidget->addTab(addStyleSheetWidget(), "StyleSheet");

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
			label->setFixedWidth(getContentWidth(label->font(), title).width() + 8);
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
			d->m_biEditRectX->setValidator(new QIntValidator(-65536, 65536, d->m_biEditRectX));
			d->m_biEditRectY->setValidator(new QIntValidator(-65536, 65536, d->m_biEditRectY));
		}
		gridLayout->addLayout(hLayout, row, 0);
		hLayout = new QHBoxLayout();
		{
			hLayout->setSpacing(0);
			hLayout->setContentsMargins(0, 0, 0, 0);
			createItemFunc(hLayout, d->m_biEditRectWidth, "Width:");
			hLayout->addSpacing(8);
			createItemFunc(hLayout, d->m_biEditRectHeight, "Height:");
			d->m_biEditRectWidth->setValidator(new QIntValidator(0, 65536, d->m_biEditRectWidth));
			d->m_biEditRectHeight->setValidator(new QIntValidator(0, 65536, d->m_biEditRectHeight));
		}
		gridLayout->addLayout(hLayout, row, 1);

		row = gridLayout->rowCount();
		d->m_biCheckBoxVisible = new QCheckBox(baseWidget);
		d->m_biCheckBoxVisible->setText("visible");
		connect(d->m_biCheckBoxVisible, &QCheckBox::stateChanged, this, &WidgetDetails::onTargetVisibelStateChanged);
		gridLayout->addWidget(d->m_biCheckBoxVisible, row, 0);

		baseLayout->addLayout(gridLayout);
		baseLayout->addStretch();
		return baseWidget;
	}

	QWidget* WidgetDetails::addAttributeInfoWidget()
	{
		d->m_attributeListWidget = new QListWidget(this);
		d->m_attributeListWidget->setObjectName("AttributeInfoWidget");
		d->m_attributeListWidget->setSortingEnabled(true);
		d->m_attributeItemMap.clear();
		
		const QMetaObject* obj = qt_getEnumMetaObject(Qt::WA_Disabled);
		const char* name = qt_getEnumName(Qt::WA_Disabled);
		QMetaEnum me = obj->enumerator(obj->indexOfEnumerator(name));
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

		return d->m_attributeListWidget;
	}

	QWidget* WidgetDetails::addStyleSheetWidget()
	{
		QWidget* widget = new QWidget(this);
		widget->setObjectName("StyleSheetWidget");
		widget->setStyleSheet("QWidget#StyleSheetWidget{background:transparent;}");
		QVBoxLayout* layout = new QVBoxLayout(widget);

		d->m_styleSheetEdit = new QTextEdit(widget);
		layout->addWidget(d->m_styleSheetEdit);

		return widget;
	}

	void WidgetDetails::applyAllWidgetInfo()
	{
		if (!checkTargetWidgetValid(true))
			return;

		applyBaseInfo();
		applyStyleSheet();
		applyAttribute();

		emit sigNeedUpdateWidget(m_targetWidget);
	}

	void WidgetDetails::updateAllWidgetInfo()
	{
		updateBaseInfo();
		updateStyleSheet();
		updateAttribute();
	}

	void WidgetDetails::applyBaseInfo(bool sendSignal, bool showErrorMsg)
	{
		if (!checkTargetWidgetValid(showErrorMsg))
			return;

		auto applyDataFunc = [=](QLineEdit* edit, std::function<QString()> getFunc, std::function<void(const QString&)> setFunc) {
			if (edit)
			{
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
			emit sigNeedUpdateWidget(m_targetWidget);
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
			emit sigNeedUpdateWidget(m_targetWidget);
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
}