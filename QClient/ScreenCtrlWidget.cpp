#include "ScreenCtrlWidget.h"

ScreenCtrlWidget::ScreenCtrlWidget(QWidget *parent)
	: QWidget(parent), _dw(0), _dh(0)
{
	ui.setupUi(this);
	setMouseTracking(true);
	installEventFilter(this);
	QObject::connect(this, &ScreenCtrlWidget::destroyed, [&] {
		//keyboard_hook::stop();
	});
}

ScreenCtrlWidget::~ScreenCtrlWidget()
{
}

void ScreenCtrlWidget::setCursor(const Rd::CursorInfo& curs)
{
	if (curs.visible)
	{
		float deltaX = (float)_img.width() / (float)_dw;
		float deltaY = (float)_img.height() / (float)_dh;

		_shapePos.setX((curs.x - curs.xhot ) * deltaX);
		_shapePos.setY((curs.y - curs.yhot)  * deltaY);
	}

	if (!curs.data.empty())
	{
		uchar* ptr = (uchar*)&curs.data[0];
		_shape = QPixmap::fromImage(QImage(ptr, curs.width, curs.height, QImage::Format_RGBA8888));
	}

}

void ScreenCtrlWidget::hideEvent(QHideEvent* event)
{
	clearFocus();
}

void ScreenCtrlWidget::showEvent(QShowEvent* event)
{
	
}

void ScreenCtrlWidget::paintEvent(QPaintEvent* event)
{
	QPainter p(this);
	//p.setRenderHint(QPainter::SmoothPixmapTransform, 1);
	p.drawImage(QPoint(0, 0), _img);
	
	if(_isShapeVisible)
		p.drawPixmap(_shapePos, _shape);
}

void ScreenCtrlWidget::setScreen(const QImage& img)
{
	_dw = img.width(); _dh = img.height();
	_img = img;
	double w = (double)img.width() / (double)width();
	double h = (double)img.height() / (double)height();

	if (img.width() > width() && img.height() > height())
	{
		if (w < h)
			_img = _img.scaledToHeight(height(), Qt::TransformationMode::SmoothTransformation);
		else
			_img = _img.scaledToWidth(width(), Qt::TransformationMode::SmoothTransformation);
	}
	update();
}


void ScreenCtrlWidget::focusInEvent(QFocusEvent* event)
{


}

void ScreenCtrlWidget::focusOutEvent(QFocusEvent* event)
{
	//keyboard_hook::stop();
}


bool ScreenCtrlWidget::eventFilter(QObject* object, QEvent* event)
{
	QMouseEvent* ev = (QMouseEvent*)event;
	float deltaX = (float)_dw / (float)_img.width();
	float deltaY = (float)_dh / (float)_img.height();


	Rd::ActionEvent::Mouse e;
	e.pressed = false;
	e.x = (float)ev->x() * deltaX;
	e.y = (float)ev->y() * deltaY;

	if (event->type() == QEvent::MouseButtonPress)
	{
		setHook();
		e.pressed = true;

		if (ev->button() == Qt::LeftButton)
			e.type = Rd::ActionEvent::Mouse::button_1;

		if (ev->button() == Qt::RightButton)
			e.type = Rd::ActionEvent::Mouse::button_2;

		if (ev->button() == Qt::MiddleButton)
			e.type = Rd::ActionEvent::Mouse::button_3;

		if (ev->button() == Qt::XButton1)
			e.type = Rd::ActionEvent::Mouse::button_4;

		if (ev->button() == Qt::XButton2)
			e.type = Rd::ActionEvent::Mouse::button_5;

		mouseEventSignal(e);

		return 1;
	}

	if (event->type() == QEvent::MouseButtonRelease )
	{
		e.pressed = false;
		if (ev->button() == Qt::LeftButton)
			e.type = Rd::ActionEvent::Mouse::button_1;

		if (ev->button() == Qt::RightButton)
			e.type = Rd::ActionEvent::Mouse::button_2;

		if (ev->button() == Qt::MiddleButton)
			e.type = Rd::ActionEvent::Mouse::button_3;

		if (ev->button() == Qt::XButton1)
			e.type = Rd::ActionEvent::Mouse::button_4;

		if (ev->button() == Qt::XButton2)
			e.type = Rd::ActionEvent::Mouse::button_5;
		
		mouseEventSignal(e);

		return 1;
	}

	if (event->type() == QEvent::MouseMove)
	{
		e.type = Rd::ActionEvent::Mouse::move;
		mouseEventSignal(e);
		return true;
	}


	if (event->type() == QEvent::Wheel)
	{
		QWheelEvent* wev = (QWheelEvent*)event;
		if (wev->delta() < 0)
			e.type = Rd::ActionEvent::Mouse::wheel_backward;
		else
			e.type = Rd::ActionEvent::Mouse::wheel_forward;
		mouseEventSignal(e);
		return true;
	}

	if (event->type() == QEvent::MouseButtonDblClick)
	{
		if (ev->button() == Qt::LeftButton)
		{
			e.type = Rd::ActionEvent::Mouse::button_1;
			e.pressed = true;
			mouseEventSignal(e);		
		}
		return true;
	}

	return false;
}

void ScreenCtrlWidget::setHook()
{
	/*keyboard_hook::start([this](uint32_t vk, uint32_t scan, bool ext, bool pressed)
		{
			uint32_t k = ScanCodeTranslator::systemToSdl(vk);
			
			return pressed;
		});*/
}

void ScreenCtrlWidget::keyPressEvent(QKeyEvent* event)
{
	uint32_t k = ScanCodeTranslator::systemToSdl(event->nativeVirtualKey());
	bool ext = false;

	if (event->key() == Qt::Key_Control)
	{
		ext = true;
		k = SDL_SCANCODE_LCTRL;
	}

	if (event->key() == Qt::Key_Shift)
	{
		ext = true;
		k = SDL_SCANCODE_LSHIFT;
	}

	if (event->key() == Qt::Key_Alt)
	{
		ext = true;
		k = SDL_SCANCODE_LALT;
		return;
	}
		
	if (event->key() == Qt::Key_End)
	{
		ext = false;
		k = SDL_SCANCODE_END;
	}
	keybdEventSignal({ k, true, ext });
}

void ScreenCtrlWidget::keyReleaseEvent(QKeyEvent* event)
{
	uint32_t k = ScanCodeTranslator::systemToSdl(event->nativeVirtualKey());
	bool ext = false;

	if (event->key() == Qt::Key_Control)
	{
		ext = true;
		k = SDL_SCANCODE_LCTRL;
	}

	if (event->key() == Qt::Key_Shift)
	{
		ext = true;
		k = SDL_SCANCODE_LSHIFT;
	}

	if (event->key() == Qt::Key_Alt)
	{
		ext = true;
		k = SDL_SCANCODE_LALT;
		return;
	}

	if (event->key() == Qt::Key_End)
	{
		ext = false;
		k = SDL_SCANCODE_END;
	}

	keybdEventSignal({ k, false, ext });
}
