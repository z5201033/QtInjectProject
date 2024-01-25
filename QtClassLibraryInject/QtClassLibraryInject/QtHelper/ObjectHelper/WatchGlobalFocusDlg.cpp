#include "WatchGlobalFocusDlg.h"
#include "../Common.h"

#include <QApplication>
#include <QCheckBox>
#include <QDebug>
#include <QPushButton>
#include <QRegularExpression>
#include <QTimer>
#include <QTime>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWindow>

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

	WatchGlobalFocusDlg::WatchGlobalFocusDlg(QWidget* parent, WatchType watchType)
		: QDialog(parent)
		, d(new WatchGlobalFocusDlgPrivate)
		, _watchType(watchType)
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
		stopWatch();
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

		switch (_watchType)
		{
		case WatchType::QWidgetFocus:
			setWindowTitle("WatchQWidgetFocus");
			break;
		case WatchType::QObjectFocus:
			setWindowTitle("WatchQObjectFocus");
			break;
		case WatchType::QWindowFocus:
			setWindowTitle("WatchQWindowFocus");
			break;
		default:
			break;
		}
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
		switch (_watchType)
		{
		case WatchType::QWidgetFocus:
			connect(qobject_cast<QApplication*>(QApplication::instance()), &QApplication::focusChanged, this, &WatchGlobalFocusDlg::onFocusChanged, Qt::UniqueConnection);
			break;
		case WatchType::QObjectFocus:
			connect(qobject_cast<QGuiApplication*>(QGuiApplication::instance()), &QGuiApplication::focusObjectChanged, this, &WatchGlobalFocusDlg::onFocusObjectChanged, Qt::UniqueConnection);
			break;
		case WatchType::QWindowFocus:
			connect(qobject_cast<QGuiApplication*>(QGuiApplication::instance()), &QGuiApplication::focusWindowChanged, this, &WatchGlobalFocusDlg::onFocusWindowChanged, Qt::UniqueConnection);
			break;
		default:
			break;
		}
	}

	void WatchGlobalFocusDlg::stopWatch()
	{
		switch (_watchType)
		{
		case WatchType::QWidgetFocus:
			disconnect(qobject_cast<QApplication*>(QApplication::instance()), &QApplication::focusChanged, this, &WatchGlobalFocusDlg::onFocusChanged);
			break;
		case WatchType::QObjectFocus:
			disconnect(qobject_cast<QGuiApplication*>(QGuiApplication::instance()), &QGuiApplication::focusObjectChanged, this, &WatchGlobalFocusDlg::onFocusObjectChanged);
			break;
		case WatchType::QWindowFocus:
			disconnect(qobject_cast<QGuiApplication*>(QGuiApplication::instance()), &QGuiApplication::focusWindowChanged, this, &WatchGlobalFocusDlg::onFocusWindowChanged);
			break;
		default:
			break;
		}
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
		if (!outputHead(now))
			return;

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

	void WatchGlobalFocusDlg::onFocusObjectChanged(QObject* object)
	{
		if (QWidget* widget = qobject_cast<QWidget*>(object)) {
			onFocusChanged(nullptr, widget);
			return;
		}
		else if (QWindow * focusWindow = qobject_cast<QWindow*>(object)) {
			onFocusWindowChanged(focusWindow);
			return;
		}
		else if (!outputHead(object))
			return;
	}

	void WatchGlobalFocusDlg::onFocusWindowChanged(QWindow* window)
	{
		if (!outputHead(window, false))
			return;

		QString output;
		QDebug debug(&output);
		debug.setVerbosity(QDebug::MaximumVerbosity);
		debug << window;
		d->_strOut += output;
	}

	bool WatchGlobalFocusDlg::outputHead(QObject* object, bool detail)
	{
		d->_outputIndex++;
		if (d->_outputIndex == s_maxOutputCount) {
			d->_strOut += "\nIs Too many data! Please clean up and try again!";
			return false;
		}

		if (!d->_timer->isActive())
			d->_timer->start();

		if (!d->_strOut.isEmpty())
			d->_strOut += '\n';

		d->_strOut += QString::number(d->_outputIndex);
		d->_strOut += QTime::currentTime().toString(" hh:mm:ss:zzz ");
		if (!object)
		{
			d->_strOut += "null";
			return false;
		}

		if (detail) 
		{
			QString nameInfo = QString("0x%0 %1")
				.arg(QString::number((quint64)object, 16))
				.arg(object->metaObject()->className());
			QString name = object->objectName();
			if (!name.isEmpty())
				nameInfo += ("(name=" + object->objectName() + ")");
			nameInfo += " ";
			d->_strOut += nameInfo;
		}

		return true;
	}
}