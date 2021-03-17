#include "GlassWidget.h"

GlassWidget::GlassWidget(const QString& title, AbstractDialogWidget* w, QWidget *parent)
	: QWidget(parent), _widget(w)
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
	
	setParent(_root);

	QObject::connect(_widget, &AbstractDialogWidget::hideDialog, [this]() { hide(); });
}

GlassWidget::~GlassWidget()
{
}



void GlassWidget::mousePressEvent(QMouseEvent* event)
{
	if(!_scrollArea->geometry().contains(event->pos()) && !_title->geometry().contains(event->pos()))
		hide();
}

void GlassWidget::resizeEvent(QResizeEvent* event)
{
	updatePosition();

	
}

void GlassWidget::paintEvent(QPaintEvent* event)
{
	QPainter p(this);
	p.fillRect(rect(), QColor(200, 200, 200, 100));
}

bool GlassWidget::eventFilter(QObject* watched, QEvent* event)
{
	//if (this->isHidden())
	//	return false;



	if (event->type() == QEvent::Move || event->type() == QEvent::Resize)
	{
		if (watched == _root)
			updatePosition();
	}

	if (event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonPress)
	{
		return 1;
	}


	return false;
}

void GlassWidget::updatePosition()
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

	if (_widget->height() > height() - y - offset)
	{
		_title->resize(_widget->width() + _scrollArea->verticalScrollBar()->width(), _title->height());
		_scrollArea->resize(_widget->width() + _scrollArea->verticalScrollBar()->width(), height() - y - offset);
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

void GlassWidget::showEvent(QShowEvent* event)
{
	/*auto* eff = graphicsEffect();
	QGraphicsBlurEffect* blur = new QGraphicsBlurEffect(this);
	blur->setBlurHints(QGraphicsBlurEffect::BlurHint::PerformanceHint);
	blur->setBlurRadius(10);
	_widget->setParent(0);
	setGraphicsEffect(blur);*/

	
	updatePosition();
	activateWindow();



}
