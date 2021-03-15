#include "GlassWidget.h"

GlassWidget::GlassWidget(AbstractDialogWidget* w, QWidget *parent)
	: QWidget(parent), _widget(w)
{
	ui.setupUi(this);
	//setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	//_widget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	_widget->setParent(this);
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
	if(!_widget->geometry().contains(event->pos()))
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


	int x = (width() - _widget->width()) / 2;
	int y = 50;
	_widget->move(x, y);
}

void GlassWidget::showEvent(QShowEvent* event)
{
	/*auto* eff = graphicsEffect();
	QGraphicsBlurEffect* blur = new QGraphicsBlurEffect(this);
	blur->setBlurHints(QGraphicsBlurEffect::BlurHint::PerformanceHint);
	blur->setBlurRadius(10);
	setGraphicsEffect(blur);

	_widget->setGraphicsEffect(eff);*/
	updatePosition();
	activateWindow();
}
