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
	InputDialogWidget(const QString& inputName, const QString& text, QWidget *parent = Q_NULLPTR);
	~InputDialogWidget();
	QString text();
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
		InputDialogWidget* w = new InputDialogWidget(inputName, text, parent);
		GlassDialogContainer* glass = new GlassDialogContainer(title, w, parent, isGlassClickable);
		QObject::connect(w, &InputDialogWidget::okPressed, [w, glass, cbck]() { cbck(w->text(), true); glass->hide(); });
		QObject::connect(w, &InputDialogWidget::cancelPressed, [w, glass, cbck]() {cbck(w->text(), false); glass->hide(); });
		glass->show();
		w->setFocus();
	}
}