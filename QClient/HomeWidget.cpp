#include "HomeWidget.h"

HomeWidget::HomeWidget(QWidget *parent)
	: TabItem(parent)
{
	ui.setupUi(this);

	_serverListWidget = new ServerListWidget(this);

	_clientListWidget = new ClientListWidgets(this);
	
	_clientControlWidget = new ClientControlWidget(this);
	
	//QWidget* w0 = new QWidget(this);

	_splitter = new QSplitter(Qt::Horizontal, this);

	_edit = new QPlainTextEdit(this);
	_edit->setReadOnly(true);
	_edit->setFocusPolicy(Qt::NoFocus);
	_serverListWidget->setFocusPolicy(Qt::NoFocus);
	_clientControlWidget->setFocusPolicy(Qt::NoFocus);


	/*QVBoxLayout* l0 = new  QVBoxLayout(this);
	l0->addWidget(_serverListWidget);
	l0->addWidget(_clientControlWidget);
	l0->setAlignment(Qt::AlignTop);
	l0->setMargin(0);
	QWidget* w = new QWidget(this);
	w->setLayout(l0);*/

	_splitter0 = new QSplitter(Qt::Vertical, this);
	_splitter0->addWidget(_clientControlWidget);
	_splitter0->addWidget(_serverListWidget);

	_splitter0->setStretchFactor(0, 100);

	_splitter1 = new QSplitter(Qt::Vertical, this);
	_splitter1->addWidget(_clientListWidget);
	
	_splitter1->setStretchFactor(0, 100);


	_splitter->addWidget(_splitter0);
	_splitter->addWidget(_splitter1);
	//_splitter->addWidget(w0);
	
	_splitter->addWidget(_edit);
_splitter->setStretchFactor(2, 100);


	QHBoxLayout* l = new QHBoxLayout;
	l->setMargin(0);
	l->addWidget(_splitter);
	setLayout(l);


	QObject::connect(_clientListWidget, &ClientListWidgets::clientChanged, [&](const Rd::Client& cl) {
		_clientControlWidget->setClient(cl);
	});

	QObject::connect(this, &HomeWidget::connectionOpen, this, &HomeWidget::onConnectionOpen);
	QObject::connect(this, &HomeWidget::connectionError, this, &HomeWidget::onConnectionError);

	EventBus::subscribe<Rd::ConnectionOpen>([this](const Rd::ConnectionOpen& ev) { connectionOpen(ev); });
	EventBus::subscribe<Rd::ConnectionError>([this](const Rd::ConnectionError& ev) { connectionError(ev); });

}

HomeWidget::~HomeWidget()
{
}



void HomeWidget::getState(QSettings& sett)
{
	sett.setValue("HomeWidget/clientsList", _clientListWidget->getState());
	sett.setValue("HomeWidget/splitter", _splitter->saveState());
	sett.setValue("HomeWidget/splitter1", _splitter1->saveState());
	sett.setValue("HomeWidget/splitter0", _splitter0->saveState());
	//sett.setValue("HomeWidget/serverList", _serverListWidget->getState());
}


void HomeWidget::setState(const QSettings& sett)
{
	_clientListWidget->setState(sett.value("HomeWidget/clientsList").toByteArray());
	//_serverListWidget->setState(sett.value("HomeWidget/serverList").toByteArray());
	_splitter->restoreState(sett.value("HomeWidget/splitter").toByteArray());
	_splitter1->restoreState(sett.value("HomeWidget/splitter1").toByteArray());
	_splitter0->restoreState(sett.value("HomeWidget/splitter0").toByteArray());
}


void HomeWidget::onConnectionError(const Rd::ConnectionError& ev)
{
	_clientControlWidget->setClient({});
	_edit->moveCursor(QTextCursor::End);
	
	QTime t = QTime::currentTime();
	_edit->insertPlainText(t.toString(Qt::ISODateWithMs) + "  ");

	_edit->insertPlainText(QString::fromLocal8Bit(ev.msg.c_str()));
	_edit->insertPlainText("\n\n");
	_edit->moveCursor(QTextCursor::End);
}

void HomeWidget::onConnectionOpen(const Rd::ConnectionOpen& ev)
{
	
	_edit->moveCursor(QTextCursor::End);
	QTime t = QTime::currentTime();
	_edit->insertPlainText(t.toString(Qt::ISODateWithMs) + "  ");

	_edit->insertPlainText("connected");
	_edit->insertPlainText("\n\n");
	_edit->moveCursor(QTextCursor::End);
}

void HomeWidget::showEvent(QShowEvent* event)
{
	QTimer::singleShot(0, _clientListWidget, SLOT(setFocus()));
}
