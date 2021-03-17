#include "GlassDialogContainer.h"

GlassDialogContainer::GlassDialogContainer(const QString& title, DialogBase* w, QWidget* parent, bool isGlassClickable)
	: QWidget(parent), _widget(w), _isGlassClickable(isGlassClickable)
{
	ui.setupUi(this);

	_title = new QWidget(this);
	_title->setMinimumHeight(_titleHeight);
	QVBoxLayout* l = new QVBoxLayout(this);
	l->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
	_title->setLayout(l);

	l->addWidget(new QLabel(title, this));

	//setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	//_widget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

	_scrollArea = new QScrollArea(this);
	_scrollArea->horizontalScrollBar()->hide();
	_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);

	setAttribute(Qt::WA_TranslucentBackground);
	_widget->setParent(_scrollArea);
	_scrollArea->setWidget(_widget);
	//_widget->setMaximumWidth(600);
	//_widget->setMaximumHeight(400);
	_root = this;
	do {
		_root = (QWidget*)_root->parent();
	} while (_root->parent());

	_root->installEventFilter(this);
	//_widget->installEventFilter(this);
	setParent(_root);

	QObject::connect(_widget, &DialogBase::showSignal, [this]() { show(); });
	QObject::connect(_widget, &DialogBase::hideSignal, [this]() { hide(); });
}

GlassDialogContainer::~GlassDialogContainer()
{
}



void GlassDialogContainer::mousePressEvent(QMouseEvent* event)
{
	if (!_isGlassClickable)
		return;
	if (!_scrollArea->geometry().contains(event->pos()) && !_title->geometry().contains(event->pos()))
		hide();
}

void GlassDialogContainer::resizeEvent(QResizeEvent* event)
{
	updatePosition();


}

void GlassDialogContainer::paintEvent(QPaintEvent* event)
{
	QPainter p(this);
	p.fillRect(rect(), QColor(200, 200, 200, 100));
}

bool GlassDialogContainer::eventFilter(QObject* watched, QEvent* event)
{
	//if (this->isHidden())
	//	return false;



	if (event->type() == QEvent::Move || event->type() == QEvent::Resize)
	{
		if (watched == _root)
			updatePosition();
	}


	/*if (event->type() == QEvent::Hide)
	{
		if (watched == _widget)
			hide();

	}

	if (event->type() == QEvent::Show)
	{
		if (watched == _widget)
			show();

	}*/


	return QObject::eventFilter(watched, event);
}

void GlassDialogContainer::updatePosition()
{
	auto r = _root->rect();
	auto p = _root->mapToGlobal({ r.left(), r.top() });
	move(0, 0);
	resize(r.width(), r.height());

	const int offset = 20;
	int x = (width() - _widget->width()) / 2;
	int y = offset;

	_title->move(x, y);
	y += _title->height();





	_scrollArea->move(x, y);

	if (_widget->height() > height() - y)
	{
		_title->resize(_widget->width(), _title->height());
		_scrollArea->resize(_widget->width(), height() - y);
	}
	else
	{
		_title->resize(_widget->width(), _title->height());
		_scrollArea->resize(_widget->width(), _widget->height());
	}

	//_scrollArea->setWidget(_widget);
/*updateGeometry();
	_widget->updateGeometry();
	_scrollArea->updateGeometry();*/
}

void GlassDialogContainer::showEvent(QShowEvent* event)
{
	/*auto* eff = graphicsEffect();
	QGraphicsBlurEffect* blur = new QGraphicsBlurEffect(this);
	blur->setBlurHints(QGraphicsBlurEffect::BlurHint::PerformanceHint);
	blur->setBlurRadius(10);
	_widget->setParent(0);
	setGraphicsEffect(blur);*/


	updatePosition();
	activateWindow();

	_widget->show();

}
