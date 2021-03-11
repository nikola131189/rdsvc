#include "LineEdit.h"

LineEdit::LineEdit(QWidget *parent)
	: QLineEdit(parent)
{
	ui.setupUi(this);
}

LineEdit::~LineEdit()
{
}

void LineEdit::focusInEvent(QFocusEvent* e)
{
	QTimer::singleShot(0, this, &QLineEdit::selectAll);
	QLineEdit::focusInEvent(e);
}


