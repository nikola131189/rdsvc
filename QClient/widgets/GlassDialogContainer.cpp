#include "GlassDialogContainer.h"

GlassDialogContainer::GlassDialogContainer(QWidget* title, QWidget* w, QWidget* parent, bool isGlassClickable)
	: QWidget(parent), _widget(w), _title(title), _isGlassClickable(isGlassClickable)
{
	ui.setupUi(this);

	_title->setParent(this);
	_scrollArea = new QScrollArea(this);
	_scrollArea->horizontalScrollBar()->hide();
	_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);

	setAttribute(Qt::WA_TranslucentBackground);
	_widget->setParent(_scrollArea);
	_scrollArea->setWidget(_widget);

	_root = this;
	do {
		_root = (QWidget*)_root->parent();
	} while (_root->parent());

	_root->installEventFilter(this);
	setParent(_root);

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
	p.fillRect(rect(), QColor(20, 20, 20, 200));
}

bool GlassDialogContainer::eventFilter(QObject* watched, QEvent* event)
{

	if (event->type() == QEvent::Move || event->type() == QEvent::Resize)
	{
		if (watched == _root)
			updatePosition();
	}
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
}

void GlassDialogContainer::showEvent(QShowEvent* event)
{
	updatePosition();
	activateWindow();
}
