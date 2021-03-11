#pragma once

#include <QTabWidget>
#include <QEvent>
#include <QMouseEvent>
#include <QObject>
#include <QTabBar>
#include <QStyleOption>
#include <QPainter>
#include "ui_TabWidget.h"


struct PinnedTab
{
	bool isPinned;
	int currentIndex;
};

Q_DECLARE_METATYPE(PinnedTab);


class TabBar : public QTabBar
{
	Q_OBJECT
public:	
	TabBar(QWidget* parent = nullptr);
	void pin(int const index);
private:
	virtual void tabLayoutChange() override;
	void rollbackLayout();
	virtual void mouseReleaseEvent(QMouseEvent* ev) override;
};



class TabWidget : public QTabWidget
{
	Q_OBJECT

public:
	TabWidget(QWidget *parent = Q_NULLPTR);
	~TabWidget();
	void pin(int index);
signals:
	void mouseRightButtonPress(const QPoint& pos);
	void newTabSignal();
private:
	void paintEvent(QPaintEvent* ev);
	virtual void keyPressEvent(QKeyEvent* ev) override;
private:
	Ui::TabWidget ui;

	TabBar* _tabBar;
};
