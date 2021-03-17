#include "ScreenPanel.h"

ScreenPanel::ScreenPanel(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	QTimer* timer = new QTimer(this);
	QObject::connect(timer, &QTimer::timeout, [this]() {
			if (utility::get_tick_count() - _lastActive > _timeout)
			{
				auto pos = this->mapFromGlobal(QCursor::pos());
				if (pos.x() < 0 || pos.x() > width() || pos.y() > height())
				{
					hide();
				}
				if (pos.y() < 0)
				{
					show();
				}
				
			}
		});
	timer->start(300);



	/*QWidget *_root = this;
	do {
		_root = (QWidget*)_root->parent();
	} while (_root->parent());


	setParent(_root);*/

	QHBoxLayout* l = new QHBoxLayout(this);

	{
		QPushButton* btn1 = new QPushButton("Screen settings", this);
		l->addWidget(btn1);
		QObject::connect(btn1, &QPushButton::clicked, this, &ScreenPanel::settingsSignal);
	}

	{
		QPushButton* btn1 = new QPushButton("Ctrl", this);
		l->addWidget(btn1);
		btn1->setCheckable(true);
		QObject::connect(btn1, &QPushButton::clicked, [this, btn1]() { buttonSignal(SDL_SCANCODE_LCTRL, btn1->isChecked(), true); });
	}

	{
		QPushButton* btn1 = new QPushButton("Shift", this);
		l->addWidget(btn1);
		btn1->setCheckable(true);
		QObject::connect(btn1, &QPushButton::clicked, [this, btn1]() { buttonSignal(SDL_SCANCODE_LSHIFT, btn1->isChecked(), true); });
	}

	{
		QPushButton* btn1 = new QPushButton("Alt", this);
		l->addWidget(btn1);
		btn1->setCheckable(true);
		QObject::connect(btn1, &QPushButton::clicked, [this, btn1]() { buttonSignal(SDL_SCANCODE_LALT, btn1->isChecked(), true); });
	}

	{
		QPushButton* btn1 = new QPushButton("Gui", this);
		l->addWidget(btn1);
		btn1->setCheckable(true);
		QObject::connect(btn1, &QPushButton::clicked, [this, btn1]() { buttonSignal(SDL_SCANCODE_LGUI, btn1->isChecked(), true);  });
	}


	{
		QPushButton* btn1 = new QPushButton("Shift + Alt", this);
		l->addWidget(btn1);
		QObject::connect(btn1, &QPushButton::clicked, [this, btn1]() {
			buttonSignal(SDL_SCANCODE_LSHIFT, true, true);
			buttonSignal(SDL_SCANCODE_LALT, true, true);
			buttonSignal(SDL_SCANCODE_LSHIFT, false, true);
			buttonSignal(SDL_SCANCODE_LALT, false, true);
		});
	}

	/*{
		QPushButton* btn1 = new QPushButton("Ctrl + Alt + Del", this);
		l->addWidget(btn1);
		QObject::connect(btn1, &QPushButton::clicked, [this, btn1]() {
			buttonSignal(SDL_SCANCODE_LCTRL, true, true);


			buttonSignal(SDL_SCANCODE_LALT, true, true);


			buttonSignal(SDL_SCANCODE_DELETE, true, false);
	


			buttonSignal(SDL_SCANCODE_LCTRL, false, true);


			buttonSignal(SDL_SCANCODE_LALT, false, true);


			buttonSignal(SDL_SCANCODE_DELETE, false, false);


			});
	}*/



	setMinimumWidth(800);
	setMaximumHeight(50);
	setLayout(l);
	
}

ScreenPanel::~ScreenPanel()
{
}

void ScreenPanel::mouseMoveEvent(QMouseEvent* event)
{
	_lastActive = utility::get_tick_count();
}

void ScreenPanel::showEvent(QShowEvent* event)
{
	_lastActive = utility::get_tick_count();
	//activateWindow();
}

void ScreenPanel::paintEvent(QPaintEvent* event)
{
	QPainter p(this);
	p.fillRect(rect(), QWidget::palette().color(QWidget::backgroundRole()));
}
