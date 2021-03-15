#pragma once

#include <QWidget>
#include "ui_ServerListWidget.h"
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QPushButton>
#include <rd/rd.h>
#include <QLabel>
#include "Storage.h"
#include "ServersEditor.h"
#include "widgets/ScrollableButtonGroup.h"
#include "widgets/GlassWidget.h"

class ServerListWidget : public QWidget
{
	Q_OBJECT

public:
	ServerListWidget(QWidget *parent = Q_NULLPTR);
	~ServerListWidget();

private:
	void loop();

	Ui::ServerListWidget ui;
	
	ScrollableButtonGroup* _serversWidget;

	std::shared_ptr<Net::Connector> _connector;
	ServerInfo _serverInfo;

	
	bool _running = true;
	
	std::shared_ptr<Net::Connection> _conn1 = nullptr, _conn2 = nullptr;
	boost::asio::io_context* _ctx;
	std::thread _thr;
	ServersEditor* _serversEditor;
	ServersModel* _model;
	GlassWidget* _glass;

	std::mutex _mut;
};
