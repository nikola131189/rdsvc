#pragma once

#include <QWidget>
#include "ui_InputDialogWidget.h"
#include "LineEdit.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include "LabelLineEdit.h"
#include "DialogBase.h"

class InputDialogWidget : public DialogBase
{
	Q_OBJECT

public:
	InputDialogWidget(const QString& title, const QString& inputName, const QString& text,
		QWidget *parent = Q_NULLPTR, bool isGlassClickable = true);
	~InputDialogWidget();
	QString text();
	LabelLineEdit* edit();
signals:
	void okPressed();
	void cancelPressed();
protected:
	void focusInEvent(QFocusEvent* event);
	bool eventFilter(QObject* watched, QEvent* event);
private:
	Ui::InputDialogWidget ui;
	LabelLineEdit* _edit;
};


#include "GlassDialogContainer.h"

namespace Gui::Dialog
{
	static void makeInput(const QString& title, const QString& inputName, const QString& text,
		QWidget* parent, const std::function<void(QString, bool)>& cbck, bool isGlassClickable = true)
	{
		InputDialogWidget* w = new InputDialogWidget(title, inputName, text, parent, isGlassClickable);

		QObject::connect(w, &InputDialogWidget::okPressed, [w, cbck]() { cbck(w->text(), true); w->hide(); });
		QObject::connect(w, &InputDialogWidget::cancelPressed, [w, cbck]() {cbck(w->text(), false); w->hide(); });
		w->show();
		w->setFocus();
	}


	static void makePassword(const QString& title, const QString& inputName, const QString& text,
		QWidget* parent, const std::function<void(InputDialogWidget *, bool)>& cbck, bool isGlassClickable = true)
	{
		InputDialogWidget* w = new InputDialogWidget(title, inputName, text, parent, isGlassClickable);
		QObject::connect(w, &InputDialogWidget::okPressed, [w, cbck]() 
			{
				cbck(w, true);
			});
		QObject::connect(w, &InputDialogWidget::cancelPressed, [w, cbck]()
			{
				cbck(w, false);
			});
		
		w->edit()->setEchoMode(QLineEdit::Password);
		w->show();
		w->setFocus();
	}
}