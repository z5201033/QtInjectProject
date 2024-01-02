#include "WatchGlobalFocusDlg.h"
#include "../Common.h"

#include <QApplication>
#include <QCheckBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QTimer>
#include <QTime>
#include <QTextEdit>
#include <QVBoxLayout>

namespace Qth
{
	static const unsigned int s_maxOutputCount = 10000;
	struct WatchGlobalFocusDlgPrivate
	{
		QPushButton*		_btnClear = nullptr;
		QCheckBox*		_checkBoxWatch = nullptr;
		QTextEdit*		_outputEditor = nullptr;

		QTimer*			_timer = nullptr;
		QString			_strOut;
		unsigned int		_outputIndex = 0;
	};

	WatchGlobalFocusDlg::WatchGlobalFocusDlg(QWidget* parent)
		: QDialog(parent)
		, d(new WatchGlobalFocusDlgPrivate)
	{
		setFocusPolicy(Qt::NoFocus);
		resize(800, 600);
		initData();
		initUI();
		initConnect();
		startWatch();
	}

	WatchGlobalFocusDlg::~WatchGlobalFocusDlg()
	{
	}

	void WatchGlobalFocusDlg::initData()
	{
		d->_timer = new QTimer(this);
		d->_timer->setInterval(100);
		d->_timer->setSingleShot(true);
		connect(d->_timer, &QTimer::timeout, this, &WatchGlobalFocusDlg::onPrint);
	}

	void WatchGlobalFocusDlg::initUI()
	{
		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->setContentsMargins(8, 8, 8, 8);
		mainLayout->setSpacing(8);
		
		QWidget* titleWidget = new QWidget(this);
		titleWidget->setFixedHeight(25);
		titleWidget->setFocusPolicy(Qt::NoFocus);
		{
			QHBoxLayout* hLayout = new QHBoxLayout(titleWidget);
			hLayout->setContentsMargins(0, 0, 0, 0);
			hLayout->setSpacing(10);

			d->_btnClear = new QPushButton(titleWidget);
			d->_btnClear->setText("Clear");
			d->_btnClear->setFixedSize(Common::getContentWidth(d->_btnClear->font(), d->_btnClear->text()).width() + 20, 20);
			d->_btnClear->setFocusPolicy(Qt::NoFocus);

			d->_checkBoxWatch = new QCheckBox(titleWidget);
			d->_checkBoxWatch->setText("Watch");
			d->_checkBoxWatch->setChecked(true);
			d->_checkBoxWatch->setFocusPolicy(Qt::NoFocus);

			hLayout->addWidget(d->_btnClear);
			hLayout->addWidget(d->_checkBoxWatch);
			hLayout->addStretch();
		}
		d->_outputEditor = new QTextEdit(this);
		d->_outputEditor->setFocusPolicy(Qt::NoFocus);
		d->_outputEditor->setReadOnly(true);
		d->_outputEditor->setLineWrapMode(QTextEdit::NoWrap);

		mainLayout->addWidget(titleWidget);
		mainLayout->addWidget(d->_outputEditor);
	}

	void WatchGlobalFocusDlg::initConnect()
	{
		connect(d->_btnClear, &QPushButton::clicked, this, &WatchGlobalFocusDlg::onClear);
		connect(d->_checkBoxWatch, &QCheckBox::stateChanged, this, [=](int state) {
			if (state == Qt::Checked)
				startWatch();
			else
				stopWatch();
		});
	}

	void WatchGlobalFocusDlg::startWatch()
	{
		connect(qobject_cast<QApplication*>(QApplication::instance()), &QApplication::focusChanged, this, &WatchGlobalFocusDlg::onFocusChanged, Qt::UniqueConnection);
	}

	void WatchGlobalFocusDlg::stopWatch()
	{
		disconnect(qobject_cast<QApplication*>(QApplication::instance()), &QApplication::focusChanged, this, &WatchGlobalFocusDlg::onFocusChanged);
	}

	void WatchGlobalFocusDlg::onPrint()
	{
		if (d->_strOut.isEmpty())
 			return;

		if (d->_outputEditor) {
			d->_outputEditor->append(d->_strOut);
		}

		d->_strOut.clear();
	}

	void WatchGlobalFocusDlg::onClear()
	{
		d->_strOut.clear();
		if (d->_outputEditor) {
			d->_outputEditor->clear();
		}
		d->_outputIndex = 0;
	}

	void WatchGlobalFocusDlg::onFocusChanged(QWidget* /*old*/, QWidget* now)
	{
		d->_outputIndex++;
		if (d->_outputIndex == s_maxOutputCount) {
			d->_strOut += "\nIs Too many data! Please clean up and try again!";
			return;
		}

		if (!d->_timer->isActive())
			d->_timer->start();

		if (!d->_strOut.isEmpty())
			d->_strOut += '\n';

		d->_strOut += QString::number(d->_outputIndex);
		d->_strOut += QTime::currentTime().toString(" hh:mm:ss:zzz ");
		if (!now)
		{
			d->_strOut += "null";
			return;
		}

		QString nameInfo = QString("0x%0 %1 %2 ")
			.arg(QString::number((quint64)now, 16))
			.arg(now->metaObject()->className())
			.arg(now->objectName());
		d->_strOut += nameInfo;

		QRect frameRect = now->frameGeometry();
		QRect clientRect = now->rect();
		QPoint posTopLeft = now->mapToGlobal(QPoint(clientRect.width() - frameRect.width(), clientRect.height() - frameRect.height()));
		frameRect.moveTo(posTopLeft);
		QString rectShow = QString("(%0,%1)(%2,%3) - w:h(%4,%5)")
			.arg(frameRect.x())
			.arg(frameRect.y())
			.arg(frameRect.right())
			.arg(frameRect.bottom())
			.arg(frameRect.width()).arg(frameRect.height());
		d->_strOut += rectShow;
	}
}