#pragma once

#include <QWidget>
#include "ui_ErrorDialog.h"
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include "DialogBase.h"
#include <QCheckBox>

class ErrorDialog : public DialogBase
{
	Q_OBJECT

public:
	ErrorDialog(QWidget *parent = Q_NULLPTR);
	~ErrorDialog();
	void showMessage(const QString& mess);
protected:
	bool eventFilter(QObject* watched, QEvent* event);
	void showEvent(QShowEvent* event);
private:
	Ui::ErrorDialog ui;
	QPlainTextEdit* _edit;
	QPushButton* _btn;
	QCheckBox* _checkBox;
};
