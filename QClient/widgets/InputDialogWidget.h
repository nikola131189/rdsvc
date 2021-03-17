#pragma once

#include <QWidget>
#include "ui_InputDialogWidget.h"
#include "AbstractDialogWidget.h"
#include "LineEdit.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include "LabelLineEdit.h"

class InputDialogWidget : public AbstractDialogWidget
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


#include "GlassWidget.h"

namespace Gui::Dialog
{
	static void makeInput(const QString& title, const QString& inputName, const QString& text, QWidget* parent, const std::function<void(QString, bool)>& cbck)
	{
		InputDialogWidget* w = new InputDialogWidget(inputName, text, parent);
		GlassWidget* glass = new GlassWidget(title, w, parent);
		QObject::connect(w, &InputDialogWidget::okPressed, [w, glass, cbck]() { cbck(w->text(), true); glass->hide(); });
		QObject::connect(w, &InputDialogWidget::cancelPressed, [w, glass, cbck]() {cbck(w->text(), false); glass->hide(); });
		glass->show();
		w->setFocus();
	}
}