#pragma once

#include <QWidget>
#include "ui_ScrollableButtonGroup.h"
#include "ScrollContainer.h"
#include <QButtonGroup>
#include <QAbstractButton>
#include <QLabel>

class ScrollableButtonGroup : public QWidget
{
	Q_OBJECT

public:
	ScrollableButtonGroup(const QString& title, QWidget *parent = Q_NULLPTR);
	~ScrollableButtonGroup();
	void addButton(QAbstractButton* btn, int id);
	void removeButton(QAbstractButton *btn);
	void clear();
signals:
	void idClicked(int id);
private:
	Ui::ScrollableButtonGroup ui;
	ScrollContainer* _container;
	QButtonGroup* _buttonGroup;
};
