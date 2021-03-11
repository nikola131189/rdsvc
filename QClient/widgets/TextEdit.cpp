#include "TextEdit.h"

TextEdit::TextEdit(QWidget *parent)
	: QTextEdit(parent)
{
	ui.setupUi(this);
}

TextEdit::~TextEdit()
{
}

void TextEdit::keyPressEvent(QKeyEvent* e)
{
	keyPressed(e->key());
	QTextEdit::keyPressEvent(e);
}
