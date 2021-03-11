#pragma once

#include <QWidget>
#include "ui_ClientControlWidget.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <qpainter>
#include <QKeyEvent>
#include <QButtonGroup>
#include <rd/rd.h>

class ClientControlWidget : public QWidget
{
	Q_OBJECT

public:
	ClientControlWidget(QWidget *parent = Q_NULLPTR);
	~ClientControlWidget();
	void setClient(const Rd::Client& cl);
	Rd::Client client() { return _client; }
signals:
	void downloadsClick();
	void terminalClick(const Rd::Client& cl);
	void filesClick(const Rd::Client& cl);
	void screenClick(const Rd::Client& cl, const Rd::ScreenInfo& scr);
	void changeClient(const Rd::Client& cl);
	void dispEvent(const Rd::DisplayEvent& ev);
public slots:
	void onDispEvent(const Rd::DisplayEvent& ev);
private:
	void clearLayout();
	void initWidgets();
	bool eventFilter(QObject* watched, QEvent* event);
private:
	Ui::ClientControlWidget ui;
	Rd::DisplayInfo _displayInfo;
	QVBoxLayout* _layout;
	std::vector<QPushButton*> _btns;
	Rd::Client _client;
};
