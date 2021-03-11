#pragma once

#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QImage>
#include <QMouseEvent>
#include <rd/rd.h>
#include "ui_ScreenCtrlWidget.h"
#include <QKeyEvent>


class ScreenCtrlWidget : public QWidget
{
	Q_OBJECT

public:
	ScreenCtrlWidget(QWidget *parent = Q_NULLPTR);
	~ScreenCtrlWidget();
	void setShapeVisible(bool visible) { _isShapeVisible = visible; }

signals:
	void keybdEventSignal(const Rd::ActionEvent::Keybd& k);
	void mouseEventSignal(const Rd::ActionEvent::Mouse& m);
public slots:
	void setScreen(const QImage& img);
	void setCursor(const Rd::CursorInfo& curs);
private:
	void hideEvent(QHideEvent* event);
	void showEvent(QShowEvent* event);
	void paintEvent(QPaintEvent* event);
	void focusInEvent(QFocusEvent* event);
	void focusOutEvent(QFocusEvent* event);
	bool eventFilter(QObject* object, QEvent* event);
	void setHook();
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);
private:
	Ui::ScreenCtrlWidget ui;

	QImage _img;
	uint32_t _dw, _dh;

	QPixmap _shape;
	QPoint _shapePos;

	bool _isShapeVisible = true;
};
