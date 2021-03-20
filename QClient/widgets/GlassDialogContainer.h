#pragma once

#include <QWidget>
#include "ui_GlassDialogContainer.h"
#include <QEvent>
#include <QVBoxLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QGraphicsBlurEffect>

#include <QScrollArea>
#include <QScrollBar>

#include <QLabel>


class GlassDialogContainer : public QWidget
{
	Q_OBJECT

public:
	GlassDialogContainer(QWidget* title, QWidget* w, QWidget* parent = Q_NULLPTR, bool isGlassClickable = true);
	~GlassDialogContainer();
private:
	void mousePressEvent(QMouseEvent* event);
	void resizeEvent(QResizeEvent* event);
	void paintEvent(QPaintEvent* event);
	bool eventFilter(QObject* watched, QEvent* event);
	void updatePosition();
	void showEvent(QShowEvent* event);

	Ui::GlassDialogContainer ui;
	QWidget* _root;
	QWidget* _title;
	QWidget* _widget;
	QScrollArea* _scrollArea;

	bool _isGlassClickable;
};



