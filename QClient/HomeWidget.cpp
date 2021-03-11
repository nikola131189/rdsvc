#include "HomeWidget.h"

HomeWidget::HomeWidget(QWidget *parent)
	: TabItem(parent)
{
	ui.setupUi(this);

	_clientListWidget = new ClientListWidgets(this);
	
	_clientControlWidget = new ClientControlWidget(this);
	_clientControlWidget->setFocusPolicy(Qt::NoFocus);

	_splitter = new QSplitter(this);

	_splitter->setOrientation(Qt::Horizontal);
	_edit = new QPlainTextEdit(this);
	_edit->setReadOnly(true);
	_edit->setFocusPolicy(Qt::NoFocus);

	_splitter->addWidget(_clientControlWidget);
	_splitter->addWidget(_clientListWidget);
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
	//sett.setValue("HomeWidget/serverList", _serverListWidget->getState());
}


void HomeWidget::setState(const QSettings& sett)
{
	_clientListWidget->setState(sett.value("HomeWidget/clientsList").toByteArray());
	//_serverListWidget->setState(sett.value("HomeWidget/serverList").toByteArray());
	_splitter->restoreState(sett.value("HomeWidget/splitter").toByteArray());
}


void HomeWidget::onConnectionError(const Rd::ConnectionError& ev)
{
	_edit->moveCursor(QTextCursor::End);

	QTime t = QTime::currentTime();
	_edit->insertPlainText(t.toString(Qt::ISODateWithMs) + "  ");

	_edit->insertPlainText(QString::fromLocal8Bit(ev.msg.c_str()) + "\n\n");
	_edit->moveCursor(QTextCursor::End);
}

void HomeWidget::onConnectionOpen(const Rd::ConnectionOpen& ev)
{
	_edit->moveCursor(QTextCursor::End);

	QTime t = QTime::currentTime();
	_edit->insertPlainText(t.toString(Qt::ISODateWithMs) + "  ");

	_edit->insertPlainText("connected\n\n");
	_edit->moveCursor(QTextCursor::End);
}

void HomeWidget::showEvent(QShowEvent* event)
{
	//QTimer::singleShot(0, _clientListWidget, SLOT(setFocus()));
}
