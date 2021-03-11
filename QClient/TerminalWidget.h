#pragma once
#include "widgets/TabItem.h"
#include <QWidget>
#include "ui_TerminalWidget.h"
#include <rd/rd.h>
#include "widgets/TextEdit.h"
#include <QTextCodec>
#include <QHBoxLayout>
#include <QTextDocument>
#include <QTextBlock >
#include "TerminalCommandWidget.h"
#include <QSplitter>
#include <QActionGroup>
#include <QMenu>




class TerminalWidget: public
	TabItem
{
	Q_OBJECT

public:
	TerminalWidget(Rd::Client cl, QWidget *parent = Q_NULLPTR);
	~TerminalWidget();
signals:
	void notify(const Rd::TerminalEvent& e);
public slots:
	void onNotify(const Rd::TerminalEvent& e);
private:
	void showSettings(const QPoint& pos);
	void resizeEvent(QResizeEvent* event);
	void showEvent(QShowEvent* event);
	void prepareToDestroy();
private:
	Rd::Client _client;
	std::string _termId, _cp;

	TextEdit* _edit;
	TerminalCommandWidget* _command;
	QSplitter* _splitter;
	boost::signals2::connection _conn;

private:
	Ui::TerminalWidget ui;
};
