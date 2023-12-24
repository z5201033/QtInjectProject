#include "QthWatchObjectFilterDlg.h"
#include "../QthCommon.h"
#include "../QthEnumUtils.h"

#include <QEvent>
#include <QListWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

namespace Qth
{
	struct WatchObjectFilterDlgPrivate
	{
		QMap<QEvent::Type, bool>			_typeFilter;
		static QMap<QEvent::Type, bool>	_typeFilterLast;

		QPushButton*		_btnCheckAll = nullptr;
		QPushButton*		_btnCheckSelected = nullptr;
		QPushButton*		_btnReset = nullptr;
		QListWidget*		_typeListWidget = nullptr;
		typedef QMap<QListWidgetItem*, QEvent::Type> ItemToType;
		ItemToType		_itemToType;
	};
	QMap<QEvent::Type, bool>	WatchObjectFilterDlgPrivate::_typeFilterLast;

	static bool defaultWatchType(QEvent::Type t) {
		static QSet<QEvent::Type> s_defaultType = {
			QEvent::MouseButtonPress,
			QEvent::MouseButtonRelease,
			QEvent::MouseButtonDblClick,
			//QEvent::MouseMove,
			QEvent::KeyPress,
			QEvent::KeyRelease,
			QEvent::FocusIn,
			QEvent::FocusOut,
			QEvent::FocusAboutToChange,
			QEvent::Enter,
			QEvent::Leave,
			QEvent::Move,
			QEvent::Resize,
			QEvent::Create,
			QEvent::Destroy,
			QEvent::Show,
			QEvent::Hide,
			QEvent::Close,
			QEvent::Quit,
			QEvent::ParentChange,
			QEvent::ParentAboutToChange,
			QEvent::ThreadChange,
			QEvent::DragEnter,
			QEvent::DragMove,
			QEvent::DragLeave
		};

		return s_defaultType.contains(t);
	}

	WatchObjectFilterDlg::WatchObjectFilterDlg(QWidget* parent)
		: QDialog(parent)
		, d(new WatchObjectFilterDlgPrivate)
	{
		resize(500, 800);
		initData();
		initUI();
		initConnect();
	}

	WatchObjectFilterDlg::~WatchObjectFilterDlg()
	{
	}

	void WatchObjectFilterDlg::initData()
	{
		if (d->_typeFilterLast.isEmpty())
			resetToDefault();
		else
			d->_typeFilter = d->_typeFilterLast;
	}

	void WatchObjectFilterDlg::initUI()
	{
		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->setContentsMargins(8, 8, 8, 8);
		mainLayout->setSpacing(8);
		
		QWidget* titleWidget = new QWidget(this);
		titleWidget->setFixedHeight(25);
		{
			QHBoxLayout* hLayout = new QHBoxLayout(titleWidget);
			hLayout->setContentsMargins(0, 0, 0, 0);
			hLayout->setSpacing(10);

			d->_btnCheckAll = new QPushButton(titleWidget);
			d->_btnCheckAll->setText("_CheckAll");
			d->_btnCheckAll->setFixedSize(QthCommon::getContentWidth(d->_btnCheckAll->font(), d->_btnCheckAll->text()).width() + 20, 20);

			d->_btnCheckSelected = new QPushButton(titleWidget);
			d->_btnCheckSelected->setText("CheckSelected");
			d->_btnCheckSelected->setFixedSize(QthCommon::getContentWidth(d->_btnCheckSelected->font(), d->_btnCheckSelected->text()).width() + 20, 20);

			d->_btnReset = new QPushButton(titleWidget);
			d->_btnReset->setText("Reset");
			d->_btnReset->setFixedSize(QthCommon::getContentWidth(d->_btnReset->font(), d->_btnReset->text()).width() + 20, 20);

			hLayout->addWidget(d->_btnCheckAll);
			hLayout->addWidget(d->_btnCheckSelected);
			hLayout->addWidget(d->_btnReset);
			hLayout->addStretch();
		}

		mainLayout->addWidget(titleWidget);
		mainLayout->addWidget(initTypeList());
	}

	void WatchObjectFilterDlg::initConnect()
	{
		connect(d->_btnCheckAll, &QPushButton::clicked, this, &WatchObjectFilterDlg::onCheckAll);
		connect(d->_btnCheckSelected, &QPushButton::clicked, this, &WatchObjectFilterDlg::onCheckSelected);
		connect(d->_btnReset, &QPushButton::clicked, this, &WatchObjectFilterDlg::onReset);
		connect(d->_typeListWidget, &QListWidget::itemChanged, this, &WatchObjectFilterDlg::onItemChanged);
	}

	QWidget* WatchObjectFilterDlg::initTypeList()
	{
		d->_typeListWidget = new QListWidget(this);
		d->_typeListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
		QMetaEnum me = QthEnumUtils::getMetaEnum(QEvent::None);
		for (int i = 0; i < me.keyCount(); ++i)
		{
			QEvent::Type t = static_cast<QEvent::Type>(me.value(i));
			QString text = QString("%0 %1").arg(me.value(i)).arg(me.key(i));

			QListWidgetItem* item = new QListWidgetItem(text);
			item->setCheckState(d->_typeFilter[t] ? Qt::Checked : Qt::Unchecked);
			item->setText(text);
			d->_typeListWidget->addItem(item);
			d->_itemToType[item] = t;
		}

		return d->_typeListWidget;
	}
	
	void WatchObjectFilterDlg::resetToDefault()
	{
		d->_typeFilter.clear();
		QMetaEnum me = QthEnumUtils::getMetaEnum(QEvent::None);
		for (int i = 0; i < me.keyCount(); ++i)
		{
			QEvent::Type t = static_cast<QEvent::Type>(me.value(i));
			d->_typeFilter[t] = defaultWatchType(t);
		}
		d->_typeFilterLast.clear();
	}

	void WatchObjectFilterDlg::onCheckAll()
	{
		bool allChecked = true;
		for (auto i : d->_typeFilter)
		{
			if (!i) {
				allChecked = false;
				break;
			}
		}

		for (auto it = d->_itemToType.begin(); it != d->_itemToType.end(); ++it)
		{
			it.key()->setCheckState(allChecked ? Qt::Unchecked : Qt::Checked);
		}
	}

	void WatchObjectFilterDlg::onCheckSelected()
	{
		QList<QListWidgetItem*> items = d->_typeListWidget->selectedItems();
		bool allChecked = true;
		for (auto i : items)
		{
			if (i->checkState() != Qt::Checked) {
				allChecked = false;
				break;
			}
		}

		for (auto i : items)
		{
			QEvent::Type t = d->_itemToType[i];
			d->_typeFilter[t] = !allChecked;
			i->setCheckState(allChecked ? Qt::Unchecked : Qt::Checked);
		}
	}

	void WatchObjectFilterDlg::onReset()
	{
		resetToDefault();
		for (auto it = d->_itemToType.begin(); it != d->_itemToType.end(); ++it)
		{
			bool checked = d->_typeFilter[it.value()];
			it.key()->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
		}
	}

	const QMap<QEvent::Type, bool>& WatchObjectFilterDlg::filterItem()
	{
		return d->_typeFilter;
	}

	void WatchObjectFilterDlg::onItemChanged(QListWidgetItem* item)
	{
		bool checkedNew = (item->checkState() == Qt::Checked);
		QEvent::Type t = d->_itemToType[item];
		bool chekcedOld = d->_typeFilter[t];

		if (chekcedOld != checkedNew)
		{
			d->_typeFilter[t] = checkedNew;
			d->_typeFilterLast = d->_typeFilter;
		}
	}
}