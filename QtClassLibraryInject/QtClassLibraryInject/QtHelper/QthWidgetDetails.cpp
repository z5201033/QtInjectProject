#include "QthWidgetDetails.h"
#include "QthCaptureDlg.h"

#include <QApplication>
#include <QClipboard>
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

namespace Qth
{
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
	{
		resize(500, 600);
		initUI();
	}

	WidgetDetails::~WidgetDetails()
	{
	}

	void WidgetDetails::setTargetWidget(QWidget* widget)
	{
		m_targetWidget = widget;
		updateAllWidgetInfo();
	}

	void WidgetDetails::initUI()
	{
		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->setSpacing(8);
		mainLayout->setContentsMargins(8, 8, 8, 8);

		m_tabWidget = new QTabWidget(this);
		m_tabWidget->setContentsMargins(0, 0, 0, 0);
		m_tabWidget->addTab(addBaseInfoWidget(), "BaseInfo");
		m_tabWidget->addTab(addAttributeInfoWidget(), "Attribute");
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

	QWidget* WidgetDetails::addBaseInfoWidget()
	{
		QWidget* widget = new QWidget(this);
		widget->setObjectName("BaseInfoWidget");
		widget->setStyleSheet("QWidget#BaseInfoWidget{background:transparent;}");
		QVBoxLayout* layout = new QVBoxLayout(widget);

		return widget;
	}

	QWidget* WidgetDetails::addAttributeInfoWidget()
	{
		m_attributeListWidget = new QListWidget(this);
		m_attributeListWidget->setObjectName("AttributeInfoWidget");
		m_attributeListWidget->setSortingEnabled(true);
		m_attributeItemMap.clear();
		
		const QMetaObject* obj = qt_getEnumMetaObject(Qt::WA_Disabled);
		const char* name = qt_getEnumName(Qt::WA_Disabled);
		QMetaEnum me = obj->enumerator(obj->indexOfEnumerator(name));
		for (int i = 0; i < me.keyCount(); ++i)
		{
			Qt::WidgetAttribute type = static_cast<Qt::WidgetAttribute>(me.value(i));
			QString text = QString("%0 %1").arg(me.value(i)).arg(me.key(i));

			QListWidgetItem* item = new AttributeListItem(type, m_attributeListWidget);
			item->setCheckState(Qt::Unchecked);
			item->setText(text);
			m_attributeListWidget->addItem(item);
			
			m_attributeItemMap[type] = item;
		}

		return m_attributeListWidget;
	}

	QWidget* WidgetDetails::addStyleSheetWidget()
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

	void WidgetDetails::applyAllWidgetInfo()
	{
		if (!m_targetWidget)
		{
			QMessageBox::information(this, "warning", "Current widget is invalid!");
			return;
		}
		applyStyleSheet();
		applyAttribute();

		emit sigNeedUpdateWidget(m_targetWidget);
	}

	void WidgetDetails::updateAllWidgetInfo()
	{
		updateStyleSheet();
		updateAttribute();
	}

	void WidgetDetails::applyStyleSheet(bool sendSignal, bool showErrorMsg)
	{
		if (!m_targetWidget)
		{
			if (showErrorMsg)
				QMessageBox::information(this, "warning", "Current widget is invalid!");
			return;
		}

		QTextEdit* textEdit = findChild<QTextEdit*>("StyleSheetEdit");
		if (textEdit)
		{
			QString text = textEdit->toPlainText();
			m_targetWidget->setStyleSheet(text);
		}

		if (sendSignal)
			emit sigNeedUpdateWidget(m_targetWidget);
	}

	void WidgetDetails::updateStyleSheet(bool showErrorMsg)
	{
		if (!m_targetWidget)
		{
			if (showErrorMsg)
				QMessageBox::information(this, "warning", "Current widget is invalid!");
			return;
		}

		QTextEdit* textEdit = findChild<QTextEdit*>("StyleSheetEdit");
		if (textEdit)
		{
			QString text = m_targetWidget ? m_targetWidget->styleSheet() : "";
			textEdit->setPlainText(text);
		}
	}

	void WidgetDetails::applyAttribute(bool sendSignal, bool showErrorMsg)
	{
		if (!m_targetWidget)
		{
			if (showErrorMsg)
				QMessageBox::information(this, "warning", "Current widget is invalid!");
			return;
		}

		auto it = m_attributeItemMap.begin();
		for (; it != m_attributeItemMap.end(); ++it)
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
		if (!m_targetWidget)
		{
			if (showErrorMsg)
				QMessageBox::information(this, "warning", "Current widget is invalid!");
			return;
		}

		auto it = m_attributeItemMap.begin();
		for (; it != m_attributeItemMap.end(); ++it)
		{
			bool src = m_targetWidget->testAttribute(it.key());
			it.value()->setCheckState(src ? Qt::Checked : Qt::Unchecked);
		}
	}
}