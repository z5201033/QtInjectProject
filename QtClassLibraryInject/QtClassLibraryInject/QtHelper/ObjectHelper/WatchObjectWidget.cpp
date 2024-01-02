#include "WatchObjectWidget.h"
#include "WatchObjectFilterDlg.h"
#include "../Common.h"
#include "../../Utils/QtEnumUtils.h"

#include <QApplication>
#include <QCheckBox>
#include <QDebug>
#include <QEvent>
#include <QLabel>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPushButton>
#include <QTimer>
#include <QTime>
#include <QTextEdit>
#include <QVBoxLayout>

namespace Qth
{
	static const unsigned int s_maxOutputCount = 100000;
	struct WatchObjectWidgetPrivate
	{
		WatchObjectFilterDlg*	_filterDlg = nullptr;
		QPushButton*				_btnFilter = nullptr;
		QPushButton*				_btnClear = nullptr;
		QCheckBox*				_checkBoxWatch = nullptr;
		QPushButton*				_btnHighlight = nullptr;
		QTextEdit*				_outputEditor = nullptr;

		QTimer*					_timer = nullptr;
		QString					_strOut;
		QDebug*					_debugOut = nullptr;
		unsigned int				_outputIndex = 0;
		bool						_needShowObject = false;
	};

	WatchObjectWidget::WatchObjectWidget(QWidget* parent)
		: QDialog(parent)
		, d(new WatchObjectWidgetPrivate)
	{
		resize(800, 600);
		initData();
		initUI();
		initConnect();
	}

	WatchObjectWidget::~WatchObjectWidget()
	{
	}

	void WatchObjectWidget::initData()
	{
		d->_debugOut = new QDebug(&d->_strOut);
		d->_timer = new QTimer(this);
		d->_timer->setInterval(300);
		connect(d->_timer, &QTimer::timeout, this, &WatchObjectWidget::onPrint);
		d->_timer->start();
	}

	void WatchObjectWidget::initUI()
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

			d->_btnFilter = new QPushButton(titleWidget);
			d->_btnFilter->setText("Filter");
			d->_btnFilter->setFixedSize(Common::getContentWidth(d->_btnFilter->font(), d->_btnFilter->text()).width() + 20, 20);

			d->_btnClear = new QPushButton(titleWidget);
			d->_btnClear->setText("Clear");
			d->_btnClear->setFixedSize(Common::getContentWidth(d->_btnClear->font(), d->_btnClear->text()).width() + 20, 20);

			d->_checkBoxWatch = new QCheckBox(titleWidget);
			d->_checkBoxWatch->setText("Watch");
			d->_checkBoxWatch->setChecked(true);

			d->_btnHighlight = new QPushButton(titleWidget);
			d->_btnHighlight->setText("Highlight");
			d->_btnHighlight->setFixedSize(Common::getContentWidth(d->_btnHighlight->font(), d->_btnHighlight->text()).width() + 20, 20);

			hLayout->addWidget(d->_btnFilter);
			hLayout->addWidget(d->_btnClear);
			hLayout->addWidget(d->_checkBoxWatch);
			hLayout->addWidget(d->_btnHighlight);
			hLayout->addStretch();
		}
		d->_outputEditor = new QTextEdit(this);
		d->_outputEditor->setFocusPolicy(Qt::NoFocus);
		d->_outputEditor->setReadOnly(true);
		d->_outputEditor->setLineWrapMode(QTextEdit::NoWrap);

		mainLayout->addWidget(titleWidget);
		mainLayout->addWidget(d->_outputEditor);

		d->_filterDlg = new WatchObjectFilterDlg(this);
	}

	void WatchObjectWidget::initConnect()
	{
		connect(d->_btnFilter, &QPushButton::clicked, this, &WatchObjectWidget::onShowFilter);
		connect(d->_btnClear, &QPushButton::clicked, this, &WatchObjectWidget::onClear);
		connect(d->_btnHighlight, &QPushButton::clicked, this, &WatchObjectWidget::onHighlight);
		connect(d->_checkBoxWatch, &QCheckBox::stateChanged, this, [=](int state) {
			if (state == Qt::Checked)
				startWatch();
			else
				stopWatch();
		});
	}

	bool WatchObjectWidget::eventFilter(QObject* watched, QEvent* event)
	{
		do 
		{
			if (!needWatch(watched, event))
				break;

			d->_outputIndex++;
			if (d->_outputIndex == s_maxOutputCount) {
				d->_strOut += "\nIs Too many data! Please clean up and try again!";
				break;
			}
			else if (d->_outputIndex > s_maxOutputCount) {
				break;
			}

			if (!d->_strOut.isEmpty())
				d->_strOut += '\n';
			
			d->_strOut += QString::number(d->_outputIndex);
			d->_strOut += QTime::currentTime().toString(" hh:mm:ss:zzz ");
			if (d->_needShowObject)
				*d->_debugOut << watched;

			formatQEvent(d->_strOut, *d->_debugOut, event);
		} while (false);

		return QDialog::eventFilter(watched, event);
	}

	void WatchObjectWidget::setFilterObject(QObject* obj)
	{
		QString titleText = "watch: ";
		if (obj)
			titleText += QString("%0(0x%1)").arg(obj->metaObject()->className()).arg(QString::number((quint64)obj, 16));
		else
			titleText += "null";
		setWindowTitle(titleText);

		stopWatch();
		_filterObj = obj;
		startWatch();
		d->_needShowObject = (obj == qApp);
	}

	void WatchObjectWidget::startWatch()
	{
		if (_filterObj)
			_filterObj->installEventFilter(this);
		if (d->_timer)
			d->_timer->start();
	}

	void WatchObjectWidget::stopWatch()
	{
		if (_filterObj)
			_filterObj->removeEventFilter(this);
		if (d->_timer)
			d->_timer->stop();
	}

	void WatchObjectWidget::onPrint()
	{
		if (d->_strOut.isEmpty())
 			return;

		if (d->_outputEditor) {
			d->_outputEditor->append(d->_strOut);
		}

		d->_strOut.clear();
	}

	void WatchObjectWidget::onClear()
	{
		d->_strOut.clear();
		if (d->_outputEditor) {
			d->_outputEditor->clear();
		}
		d->_outputIndex = 0;
	}

	void WatchObjectWidget::onHighlight()
	{
		emit sigHighlight(_filterObj.data());
	}

	void WatchObjectWidget::onShowFilter()
	{
		if (d->_filterDlg)
			d->_filterDlg->exec();
	}

	bool WatchObjectWidget::needWatch(QObject* watched, QEvent* event)
	{
		if (watched == this && watched == d->_timer && watched == d->_outputEditor)
			return false;

		if (d->_filterDlg)
		{
			QEvent::Type t = event->type();
			if (t >= QEvent::User)
				return d->_filterDlg->filterItem().contains(QEvent::User);
			return d->_filterDlg->filterItem().value(t);
		}

		return true;
	}

	void WatchObjectWidget::formatQEvent(QString& strOut, QDebug& debutOut, QEvent* event)
	{
		d->_strOut += QtEnumUtils::getEnumName(event->type(), false);
		d->_strOut += " ";
		*d->_debugOut << event;
	}
}