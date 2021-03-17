#pragma once

#include <QWidget>
#include "ui_GlassWidget.h"
#include <QEvent>
#include <QVBoxLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QGraphicsBlurEffect>
#include "AbstractDialogWidget.h"
#include <QScrollArea>
#include <QScrollBar>

#include <QLabel>

class GlassWidget : public QWidget
{
	Q_OBJECT

public:
	GlassWidget(const QString& title, AbstractDialogWidget*w, QWidget *parent = Q_NULLPTR);
	~GlassWidget();
private:
	void mousePressEvent(QMouseEvent* event);
	void resizeEvent(QResizeEvent* event);
	void paintEvent(QPaintEvent* event);
	bool eventFilter(QObject* watched, QEvent* event);
	void updatePosition();
	void showEvent(QShowEvent* event);

	Ui::GlassWidget ui;
	QWidget* _root;
	QWidget* _title;
	AbstractDialogWidget* _widget;
	QScrollArea* _scrollArea;

	const int _titleHeight = 50;
};
