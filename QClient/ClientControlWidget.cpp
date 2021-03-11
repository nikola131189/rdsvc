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

	for (size_t i = 0; i < 20; i++)
	{
		QPushButton* btn = new QPushButton(this);

		btn->setFocusPolicy(Qt::NoFocus);
		_btns.push_back(btn);
		QObject::connect(btn, &QPushButton::clicked, [this, i]() {
			if (i == 0) downloadsClick();
			if (i == 1) terminalClick(_client);
			if (i == 2) filesClick(_client);
			if (i > 2 && i - 3 < _displayInfo.screens.size())
			{
				screenClick(_client, _displayInfo.screens[i - 3]);
			}
		});
		if(i > 0)
			btn->hide();
		_layout->addWidget(btn);

	}
	_btns[0]->setText("downloads");
}

ClientControlWidget::~ClientControlWidget()
{
}


void ClientControlWidget::setClient(const Rd::Client& cl)
{
	clearLayout();
	_client = cl;
	_displayInfo = Rd::DisplayInfo();

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

void ClientControlWidget::clearLayout()
{

	for (int i = 1; i < _btns.size(); i++)
		_btns[i]->hide();
}

void ClientControlWidget::initWidgets()
{
	clearLayout();

	_btns[0]->setText("downloads");
	_btns[0]->show();
	_btns[1]->setText("terminal");
	_btns[1]->show();
	_btns[2]->setText("files");
	_btns[2]->show();

	for (size_t i = 0; i < _displayInfo.screens.size(); i++)
	{
		if(i >= _btns.size())
			break;
		_btns[i + 3]->show();
		_btns[i + 3]->setText(std::to_string(_displayInfo.screens[i].id).c_str());
	}
}