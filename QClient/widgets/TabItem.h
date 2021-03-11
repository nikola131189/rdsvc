#pragma once

#include <QWidget>
#include "ui_TabItem.h"
#include <QSettings>
#include <QStyleOption>
#include <QPainter>
class TabItem : public QWidget
{
	Q_OBJECT

public:
	TabItem(QWidget *parent = Q_NULLPTR);
	~TabItem();
	virtual void showSettings(const QPoint& pos) {}
	virtual void prepareToDestroy() {}
	virtual void getState(QSettings& sett) {}
	virtual void setState(const QSettings& sett) {}
private:
	void paintEvent(QPaintEvent* event);
	Ui::TabItem ui;
};
