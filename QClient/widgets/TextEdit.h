#pragma once

#include <QTextEdit>
#include "ui_TextEdit.h"
#include <QKeyEvent>

class TextEdit : public QTextEdit
{
	Q_OBJECT

public:
	TextEdit(QWidget *parent = Q_NULLPTR);
	~TextEdit();
signals:
	void keyPressed(int key);
private:
	void keyPressEvent(QKeyEvent* e);
private:
	Ui::TextEdit ui;
};
