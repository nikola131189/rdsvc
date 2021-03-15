#include "ServerListWidget.h"

ServerListWidget::ServerListWidget(QWidget *parent)
	:
	QWidget(parent) 

{
	ui.setupUi(this);
	_serversWidget = new ScrollableButtonGroup("Servers", this);


	_model = new ServersModel(this);

	

	QObject::connect(_model, &QAbstractItemModel::modelReset, [this]() {
			_serversWidget->clear();

			
			for (int i = 0; i < _model->servers().size(); i++)
			{
				QPushButton* btn = new QPushButton(_model->servers()[i].description.c_str(), this);
				_serversWidget->addButton(btn, i);
				btn->setCheckable(true);
			}
		});

	QObject::connect(_model, &QAbstractItemModel::dataChanged, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight) {
		_serversWidget->clear();


		for (int i = 0; i < _model->servers().size(); i++)
		{
			QPushButton* btn = new QPushButton(_model->servers()[i].description.c_str(), this);
			_serversWidget->addButton(btn, i);
			btn->setCheckable(true);
		}
	});


	

	{
		for (int i = 0; i < _model->servers().size(); i++)
		{
			QPushButton* btn = new QPushButton(_model->servers()[i].description.c_str(), this);
			btn->setCheckable(true);
			_serversWidget->addButton(btn, i);
		}
	}


	_serversEditor = new ServersEditor(_model);
	_glass = new GlassWidget(_serversEditor, this);
	
	//_serversEditor->hide();
	//_serversEditor->setWindowFlags(Qt::Dialog);
	
	_glass->hide();

	QObject::connect(_serversWidget, &ScrollableButtonGroup::idClicked, [this](int id) {


		if (_conn1) _conn1->stop();
		if (_conn2) _conn2->stop();

		if (_connector) _connector->cancel();
		_connector = nullptr;
		_serverInfo = _model->servers()[id];
		if (_serverInfo.connectionType == "tcp")
			_connector = std::make_shared<Net::ConnectorTcp>(_serverInfo.address1, _serverInfo.port1);

		if (_serverInfo.connectionType == "socks5")
			_connector = std::make_shared<Net::ConnectorSocks5>(_serverInfo.address1, _serverInfo.port1, _serverInfo.address2, _serverInfo.port2);
	});


	auto *layout = new QVBoxLayout(this);
	layout->setAlignment(Qt::AlignVCenter | Qt::AlignTop);
	layout->setContentsMargins(5, 5, 5, 5);
	
	setLayout(layout);

	layout->addWidget(_serversWidget);

	QPushButton* btn = new QPushButton("edit", this);
	QObject::connect(btn, &QPushButton::clicked, [this]() { _glass->show(); });
	layout->addWidget(btn);
	_thr = std::thread(&ServerListWidget::loop, this);
}

ServerListWidget::~ServerListWidget()
{
	exit(0);
	_running = false;
	if (_conn1) _conn1->stop();
	if (_conn2) _conn2->stop();
	_ctx.stop();
	if(_thr.joinable())
		_thr.join();
}

void ServerListWidget::loop()
{

	Rd::Inet::init("", utility::gen_uuid());

	boost::asio::signal_set sign(_ctx, SIGINT, SIGTERM);
	sign.async_wait([&](auto, auto) {
		_running = false;
		_ctx.stop();
		});


	while (_running)
	{
		if (_connector)
		{
				
			if (!_conn1 || !_conn1->isOpen() || utility::get_tick_count() - _conn1->lastActive() > 5000)
			{

				boost::system::error_code ec;
				auto sock = _connector->connect(_ctx, ec);
				if (!ec)
				{
					_conn1 = std::make_shared<Net::Connection>(Rd::Inet::sess1(), std::move(sock), _serverInfo.secret, crypto::RsaEncryptor());
					_conn1->start();
				}
			}


			if (!_conn2 || !_conn2->isOpen() || utility::get_tick_count() - _conn2->lastActive() > 5000)
			{
				boost::system::error_code ec;
				auto sock = _connector->connect(_ctx, ec);
				if (!ec)
				{
					_conn2 = std::make_shared<Net::Connection>(Rd::Inet::sess2(), std::move(sock), _serverInfo.secret, crypto::RsaEncryptor());
					_conn2->start();
				}
			}
		}
		_ctx.run_for(std::chrono::milliseconds(500));
	}
}
