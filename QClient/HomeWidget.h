#pragma once

#include <QWidget>
#include "ui_HomeWidget.h"
#include "ClientListWidgets.h"
#include "QVBoxLayout"
#include <QSettings>
#include "ClientControlWidget.h"
#include <QSplitter>
#include <QPlainTextEdit>
#include "widgets/TabItem.h"
#include <QTime>
#include "ServerListWidget.h"

class HomeWidget : public TabItem
{
	Q_OBJECT

public:
	HomeWidget(QWidget *parent = Q_NULLPTR);
	~HomeWidget();
	void getState(QSettings& sett);
	void setState(const QSettings& sett);
	ClientControlWidget* control() { return _clientControlWidget; }
signals:
	void connectionError(const Rd::ConnectionError& ev);
	void connectionOpen(const Rd::ConnectionOpen& ev);
public slots:
	void onConnectionError(const Rd::ConnectionError& ev);
	void onConnectionOpen(const Rd::ConnectionOpen& ev);
private:
	void showEvent(QShowEvent* event);
	QSplitter* _splitter, *_splitter1, *_splitter0;
	QPlainTextEdit* _edit;
	Ui::HomeWidget ui;
	ClientListWidgets* _clientListWidget;
	ClientControlWidget* _clientControlWidget;
	ServerListWidget* _serverListWidget;
};
