#include "ScreenWidget.h"

ScreenWidget::ScreenWidget(const Rd::Client& cl, const Rd::ScreenInfo& scr, QWidget* parent)
	: _client(cl), _screenInfo(scr), TabItem(parent)
{
	ui.setupUi(this);
	_screenSettings = new ScreenSettingsWidget(scr, this);
	_glass = new GlassWidget(_screenSettings, this);


	QObject::connect(_screenSettings, &ScreenSettingsWidget::paramChanged, [&](const codec::VideoFormat& fmt) {
		sendClose();
		sendOpen(fmt);
		});



	_screenCtrlWidget = new ScreenCtrlWidget(this);
	setFocusPolicy(Qt::FocusPolicy::ClickFocus);
	_screenCtrlWidget->setFocusPolicy(Qt::ClickFocus);
	setFocusPolicy(Qt::NoFocus);

	QObject::connect(this, &ScreenWidget::setScreen, _screenCtrlWidget, &ScreenCtrlWidget::setScreen);
	QObject::connect(this, &ScreenWidget::setCursor, _screenCtrlWidget, &ScreenCtrlWidget::setCursor);

	QHBoxLayout* l = new QHBoxLayout(this);
	l->setMargin(0);
	l->addWidget(_screenCtrlWidget);
	setLayout(l);


	QObject::connect(_screenCtrlWidget, &ScreenCtrlWidget::keybdEventSignal, [&](const Rd::ActionEvent::Keybd& k) {
		if (_screenSettings->isSpyMode()) return;
		Rd::ActionEvent ev;
		ev.val = k;
		ev.screenId = _screenInfo.id;
		Rd::Inet::send(_client.id, ev);
		});


	QObject::connect(_screenCtrlWidget, &ScreenCtrlWidget::mouseEventSignal, [&](const Rd::ActionEvent::Mouse& m) {
		if (_screenSettings->isSpyMode()) return;
		Rd::ActionEvent ev;
		ev.val = m;
		ev.screenId = _screenInfo.id;
		Rd::Inet::send(_client.id, ev);
		});

	

	QClipboard* c = QApplication::clipboard();
	QObject::connect(c, &QClipboard::dataChanged, this, &ScreenWidget::onClipboardChanged);

	QObject::connect(this, &ScreenWidget::notifyClipboard, this, &ScreenWidget::onNotifyClipboard);

	_screenConn = EventBus::subscribe<Rd::VideoEvent>(std::bind(&ScreenWidget::notifyScreen, this, std::placeholders::_1));
	_notifyClipboardConn = EventBus::subscribe<Rd::ClipboardEvent>(std::bind(&ScreenWidget::notifyClipboard, this, std::placeholders::_1));


	_upperPanel = new ScreenPanel(_screenCtrlWidget);
	_upperPanel->hide();

	QObject::connect(_screenCtrlWidget, &ScreenCtrlWidget::upperPanelShow, [this]() {
		_upperPanel->show();
	});

	QObject::connect(_upperPanel, &ScreenPanel::settingsSignal, [this]() {_glass->show(); });

	_glass->show();
}

ScreenWidget::~ScreenWidget()
{
}


void ScreenWidget::notifyScreen(const Rd::VideoEvent& ev)
{
		if (_client.id != ev.source || !this->isVisible())
			return;
		if (_fmt != ev.videoFormat || !_decoder)
		{
			_fmt = ev.videoFormat;
			_decoder = codec::CodecFactory::makeDecoder(_fmt);
			_decoder->init(_fmt);
			_buffer.resize(_fmt.dw * _fmt.dh * 3);
		}
		if (!ev.data.empty())
		{
			_decoder->decode(&ev.data[0], ev.data.size(), &_buffer[0], _buffer.size());
			QImage im(&_buffer[0], _fmt.dw, _fmt.dh, QImage::Format_RGB888);
			setScreen(im);
		}

		setCursor(ev.cursor);
}

void ScreenWidget::onNotifyClipboard(const Rd::ClipboardEvent& ev)
{
	if (_client.id != ev.source || !this->isVisible() || !_screenSettings->isClipboardSync())
		return;

	_clipboardLock = true;

	QClipboard* c = QApplication::clipboard();


	QString str = QString::fromUtf8(ev.text.c_str());
	QString s = c->text();
	if (str != s)
	{
		c->setText(str);
	}
	
}


void ScreenWidget::showSettings(const QPoint& pos)
{
	
}

void ScreenWidget::showEvent(QShowEvent* event)
{
	if (_buffer.empty())
		showSettings(mapToGlobal({ 0, 0 }));
	else
		sendOpen(_screenSettings->getVideoFormat());
}

void ScreenWidget::hideEvent(QHideEvent* event)
{
	sendClose();
}

void ScreenWidget::sendOpen(const codec::VideoFormat& f)
{
	{
		Rd::DisplayEvent ev;
		auto val = Rd::DisplayEvent::Open();
		val.blockInput = _screenSettings->isDisableInput();
		val.screenId = _screenInfo.id;
		val.videoFormat = f;
		val.videoFormat.dw = codec::closet_multiple(f.w, 16);
		val.videoFormat.dh = codec::closet_multiple(f.h, 16);

		_screenCtrlWidget->setShapeVisible(_screenSettings->isCursorVisible());

		ev.val = val;
		Rd::Inet::send(_client.id, ev);
	}
}

void ScreenWidget::sendClose()
{
	Rd::DisplayEvent ev;
	auto val = Rd::DisplayEvent::Close();
	val.screenId = _screenInfo.id;
	ev.val = val;
	Rd::Inet::send(_client.id, ev);
}

void ScreenWidget::prepareToDestroy()
{
	_screenConn.disconnect();
	_notifyClipboardConn.disconnect();
	//Obs::unsubscribe((ObServer<Rd::ClipboardEvent>*)this);
}

void ScreenWidget::resizeEvent(QResizeEvent* event)
{
	int x = (width() - _upperPanel->width()) / 2;
	int y = 0;
	_upperPanel->move(x, y);
}


void ScreenWidget::onClipboardChanged()
{
	if (!isVisible() || !_screenSettings->isClipboardSync())
		return;

	if (_clipboardLock)
	{
		_clipboardLock = false;
		return;
	}
	
	QClipboard* c = QApplication::clipboard();


	if (const QMimeData* m = c->mimeData())
	{
		QThread::msleep(1);
		Rd::ClipboardEvent ev;
		ev.text = c->text().toUtf8().toStdString();
		if (!ev.text.empty())
			Rd::Inet::send(_client.id, ev);
	}
}