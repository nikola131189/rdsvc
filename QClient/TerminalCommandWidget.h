#pragma once
#include <QWidget>
#include <QKeyEvent>
#include <QPlainTextEdit>
#include <QWidget>
#include "TerminalHistory.h"
#include "ui_TerminalCommandWidget.h"

class TerminalCommandWidget : public QTextEdit
{
	Q_OBJECT

public:
	TerminalCommandWidget(QWidget *parent = Q_NULLPTR);
	~TerminalCommandWidget();
public:
signals:
	void command(const QString& str);
	void cancel();
	void pauseBreak();
private:
	void keyPressEvent(QKeyEvent* e);
	int _cur;
private:
	Ui::TerminalCommandWidget ui;
};
