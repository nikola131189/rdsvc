#include "TerminalWidget.h"

TerminalWidget::TerminalWidget(Rd::Client cl, QWidget *parent)
	: TabItem(parent), _client(cl), _cp("CP866")
{
	ui.setupUi(this);

	_termId = utility::gen_uuid();
	_edit = new TextEdit(this);
	_edit->setReadOnly(true);
	_edit->setFocusPolicy(Qt::NoFocus);

	Rd::TerminalEvent ev;
	ev.id = _termId;
	ev.type = Rd::TerminalEvent::OPEN;
	Rd::Inet::send(_client.id, ev);

	_command = new TerminalCommandWidget(this);
	


	QObject::connect(_command, &TerminalCommandWidget::command, [this](const QString& arg) {
			Rd::TerminalEvent ev;
			ev.id = _termId;
			ev.type = Rd::TerminalEvent::STDIN;
			ev.data = utility::conv<char>(arg.toUtf8().toStdString(), _cp, "utf-8");
			Rd::Inet::send(_client.id, ev);
		});


	QObject::connect(_command, &TerminalCommandWidget::cancel, [this]() {
			Rd::TerminalEvent ev;
			ev.id = _termId;
			ev.type = Rd::TerminalEvent::CTRLC;
			Rd::Inet::send(_client.id, ev);
		});




	_splitter = new QSplitter(this);
	_splitter->setOrientation(Qt::Vertical);
	_splitter->addWidget(_edit);
	_splitter->addWidget(_command);
	_splitter->setStretchFactor(0, 100);

	_splitter->setSizes({ height() - 50, 50 });

	QHBoxLayout* l = new QHBoxLayout(this);
	l->addWidget(_splitter);
	l->setMargin(0);
	setLayout(l);

	QObject::connect(this, &TerminalWidget::notify, this, &TerminalWidget::onNotify);

	_conn = EventBus::subscribe<Rd::TerminalEvent>(std::bind(&TerminalWidget::notify, this, std::placeholders::_1));
}

TerminalWidget::~TerminalWidget()
{
	Rd::TerminalEvent ev;
	ev.id = _termId;
	ev.type = Rd::TerminalEvent::CLOSE;
	Rd::Inet::send(_client.id, ev);
}


void TerminalWidget::showSettings(const QPoint& pos)
{
	QActionGroup* group = new QActionGroup(this);

	QMenu* menu = new QMenu();
	{
		QAction* act = menu->addAction("CP866", [&]() { _cp = "CP866"; });
		act->setCheckable(true);
		if (_cp == "CP866")
			act->setChecked(true);
		act->setActionGroup(group);
	}


	{
		QAction* act = menu->addAction("CP1251", [&]() { _cp = "CP1251"; });
		act->setCheckable(true);
		if (_cp == "CP1251")
			act->setChecked(true);
		act->setActionGroup(group);
	}

	{
		QAction* act = menu->addAction("UTF-8", [&]() { _cp = "UTF-8"; });
		act->setCheckable(true);
		if (_cp == "UTF-8")
			act->setChecked(true);
		act->setActionGroup(group);
	}

	menu->exec(pos);
}


void TerminalWidget::resizeEvent(QResizeEvent* event)
{
	
}


void TerminalWidget::showEvent(QShowEvent* event)
{
	_command->setFocus();
}


void TerminalWidget::prepareToDestroy()
{
	_conn.disconnect();
	Rd::TerminalEvent ev;
	ev.id = _termId;
	ev.type = Rd::TerminalEvent::CLOSE;
	Rd::Inet::send(_client.id, ev);
}

void TerminalWidget::onNotify(const Rd::TerminalEvent& e)
{
	if (e.id != _termId) return;

	_edit->moveCursor(QTextCursor::End);

	QString unicode;
	if (e.type == Rd::TerminalEvent::STDOUT)
	{
		_edit->setTextColor(qRgb(238, 237, 240));
		unicode = QString::fromUtf8(utility::conv<char>(e.data, "UTF-8", _cp).c_str());
	}

	if (e.type == Rd::TerminalEvent::STDERR)
	{
		_edit->setTextColor(qRgb(255, 0, 0));
		unicode = QString::fromUtf8(utility::conv<char>(e.data, "UTF-8", _cp).c_str());
	}

	if (e.type == Rd::TerminalEvent::ERR)
	{
		_edit->setTextColor(qRgb(255, 255, 0));
		_edit->insertPlainText("\n");
		_edit->insertPlainText(QString::fromUtf8(e.ec.message.c_str()) + ": " + std::to_string(e.ec.value).c_str() + "\n");

	}
	_edit->insertPlainText(unicode);
	_edit->moveCursor(QTextCursor::End);
}
