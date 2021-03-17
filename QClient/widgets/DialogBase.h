#pragma once

#include <QWidget>
#include "ui_DialogBase.h"

class DialogBase : public QWidget
{
	Q_OBJECT

public:
	DialogBase(QWidget *parent = Q_NULLPTR);
	~DialogBase();
signals:
	void showSignal();
	void hideSignal();
private:
	Ui::DialogBase ui;
};
