#pragma once

#include <QWidget>
#include "ui_ScreenPanel.h"
#include <QTimer>
#include <rd/rd.h>
#include <QPainter>
#include <QPushButton>
#include <QHBoxLayout>


class ScreenPanel : public QWidget
{
	Q_OBJECT

public:
	ScreenPanel(QWidget *parent = Q_NULLPTR);
	~ScreenPanel();
signals:
	void settingsSignal();
	void shortcutSignal(const QString& shortcut);
	void buttonSignal(uint32_t k, bool pressed, bool ext);
protected:
	void mouseMoveEvent(QMouseEvent* event);
	void showEvent(QShowEvent* event);
	void paintEvent(QPaintEvent* event);
private:
	Ui::ScreenPanel ui;
	const uint32_t _timeout = 500;
	uint64_t _lastActive = 0;
};
