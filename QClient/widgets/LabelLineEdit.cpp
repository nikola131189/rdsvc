#include "LabelLineEdit.h"

LabelLineEdit::LabelLineEdit(const QString& title, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	_label = new QLabel(title, this);
	_edit = new LineEdit(this);
	QVBoxLayout* l = new QVBoxLayout(this);
	l->addWidget(_label);
	l->addWidget(_edit);
	setLayout(l);
}

LabelLineEdit::~LabelLineEdit()
{
}

QString LabelLineEdit::text()
{
	return _edit->text();
}

void LabelLineEdit::setText(const QString& txt)
{
	_edit->setText(txt);
}

void LabelLineEdit::setEchoMode(QLineEdit::EchoMode m)
{
	_edit->setEchoMode(m);
}

void LabelLineEdit::setLabelText(const QString& txt)
{
	_label->setText(txt);
}

void LabelLineEdit::focusInEvent(QFocusEvent* event)
{
	_edit->setFocus();
}
