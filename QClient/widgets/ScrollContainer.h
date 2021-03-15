#pragma once

#include <QWidget>
#include "ui_ScrollContainer.h"
#include <QScrollArea>
#include <QVBoxLayout>
#include <QScrollBar>

class ScrollContainer : public QScrollArea
{
	Q_OBJECT

public:
	ScrollContainer(QWidget *parent = Q_NULLPTR);
	~ScrollContainer();
	void clear();
	void addWidget(QWidget* w);
	void removeWidget(QWidget* w);
private:
	void resizeEvent(QResizeEvent* ev);
	void drawWidgets();
	Ui::ScrollContainer ui;
	std::vector<QWidget*> _widgets;
};
