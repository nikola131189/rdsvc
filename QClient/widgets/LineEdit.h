#pragma once

#include <QLineEdit>
#include <QTimer>
#include "ui_LineEdit.h"

class LineEdit : public QLineEdit
{
	Q_OBJECT

public:
	LineEdit(QWidget *parent = Q_NULLPTR);
	~LineEdit();

private:
	virtual void focusInEvent(QFocusEvent* e) override;
private:
	Ui::LineEdit ui;
};
