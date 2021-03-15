#pragma once

#include <QWidget>
#include "ui_AbstractSettingsWidget.h"

#include <QEvent>
#include <QGraphicsBlurEffect>
#include <QPainter>



class AbstractDialogWidget : public QWidget
{
	Q_OBJECT

public:
	AbstractDialogWidget(QWidget *parent = Q_NULLPTR);
	~AbstractDialogWidget();
signals:
	void hideDialog();
private:
	void paintEvent(QPaintEvent* ev);
	Ui::AbstractSettingsWidget ui;
};
