#pragma once

#include <QWidget>
#include "ui_LabelLineEdit.h"
#include "LineEdit.h"
#include <QLabel>
#include <QVBoxLayout>

class LabelLineEdit : public QWidget
{
	Q_OBJECT

public:
	LabelLineEdit(const QString& title, QWidget *parent = Q_NULLPTR);
	~LabelLineEdit();
	QString text();
	void setText(const QString& txt);
protected:
	void focusInEvent(QFocusEvent* event);
private:
	Ui::LabelLineEdit ui;
	QLabel* _label;
	LineEdit* _edit;
};
