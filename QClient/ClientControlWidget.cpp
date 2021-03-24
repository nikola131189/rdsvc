#include "ClientControlWidget.h"

ClientControlWidget::ClientControlWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	QObject::connect(this, &ClientControlWidget::dispEvent, this, &ClientControlWidget::onDispEvent);

	EventBus::subscribe<Rd::DisplayEvent>(std::bind(&ClientControlWidget::dispEvent, this, std::placeholders::_1));

	_layout = new QVBoxLayout;
	_layout->setAlignment(Qt::AlignVCenter | Qt::AlignTop);
	_layout->setContentsMargins(5, 5, 5, 5);
	setLayout(_layout);


	_btns = new ScrollableButtonGroup("Controls", this);
	_layout->addWidget(_btns);



	QObject::connect(_btns, &ScrollableButtonGroup::idClicked, [this](int id) {
		if (id == 0) downloadsClick();
		if (id == 1) terminalClick(_client);
		if (id == 2) filesClick(_client);
		if (id > 2 && id - 3 < _displayInfo.screens.size())
		{
			screenClick(_client, _displayInfo.screens[id - 3]);
		}
		});

	initWidgets();
}

ClientControlWidget::~ClientControlWidget()
{
}


void ClientControlWidget::setClient(const Rd::Client& cl)
{
	_client = cl;
	_displayInfo = Rd::DisplayInfo();
	initWidgets();
	if (cl.id == 0)
		return;

	Rd::DisplayEvent ev;
	ev.val = Rd::DisplayEvent::Request();
	Rd::Inet::send(_client.id, ev);
	changeClient(cl);
}


void ClientControlWidget::onDispEvent(const Rd::DisplayEvent& ev)
{
	if (ev.source != _client.id) return;
	if (auto pval = std::get_if<Rd::DisplayEvent::Response>(&ev.val))
	{
		_displayInfo = pval->display;
		initWidgets();
	}
}



void ClientControlWidget::initWidgets()
{
	
	_btns->clear();
	
	{
		QPushButton* b = new QPushButton("downloads", this);
		b->setDefault(true);
		_btns->addButton(b, 0);
	}
	if (_client.id != 0)
	{
		{
			QPushButton* b = new QPushButton("terminal", this);
			b->setDefault(true);
			_btns->addButton(b, 1);
		}
		{
			QPushButton* b = new QPushButton("files", this);
			b->setDefault(true);
			_btns->addButton(b, 2);
		}
	}
	


	for (size_t i = 0; i < _displayInfo.screens.size(); i++)
	{
		QPushButton* b = new QPushButton(std::to_string(_displayInfo.screens[i].id).c_str(), this);
		b->setDefault(true);
		_btns->addButton(b, i + 3);
	}

}

bool ClientControlWidget::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent* ev = (QKeyEvent*)event;
		if (ev->key() == Qt::Key_T && ev->modifiers() == Qt::ControlModifier)
		{
			terminalClick(_client);
		}

		if (ev->key() == Qt::Key_F && ev->modifiers() == Qt::ControlModifier)
		{
			QTimer::singleShot(10, [this]() {filesClick(_client);});
			
		}

		/*if (ev->key() > Qt::Key_2 && ev->key() <= Qt::Key_9)
		{
			int k = ev->key() - Qt::Key_3;
			if (k < _displayInfo.screens.size())
			{
				event_bus::post(OpenScreenEvent(_client, _displayInfo.screens[k]));
			}
		}*/


	}
	return false;
}
