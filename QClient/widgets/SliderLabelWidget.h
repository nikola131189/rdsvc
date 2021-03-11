#pragma once

#include <QWidget>
#include "ui_SliderLabelWidget.h"
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QGridLayout>

class SliderLabelWidget : public QWidget
{
	Q_OBJECT

public:
	SliderLabelWidget(const QString& text, int min, int max,
		Qt::Orientation orientation = Qt::Vertical, QWidget *parent = Q_NULLPTR);
	~SliderLabelWidget();
	int value();
	void setValue(int v);
signals:
	void valueChanged();
private:
	Ui::SliderLabelWidget ui;
	QLabel* _label1;
	QLabel* _label2;
	QSlider* _slider;

	QString _text;
};
