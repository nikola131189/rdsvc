#include "TerminalCommandWidget.h"

TerminalCommandWidget::TerminalCommandWidget(QWidget *parent)
	: QTextEdit(parent), _cur(-1)
{
	ui.setupUi(this);
}

TerminalCommandWidget::~TerminalCommandWidget()
{
}



void TerminalCommandWidget::keyPressEvent(QKeyEvent* e)
{

	if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
	{
		QString str = document()->toPlainText();

		if (str != "")
		{
			TerminalHistory::push(str);
		}
		_cur = -1;
		command(str);
		clear();
		moveCursor(QTextCursor::Start);
		return;
	}

	if (e->key() == Qt::Key_Down)
	{
		if (_cur < -1) return;
		if (_cur != -1) _cur--;
		document()->setPlainText(TerminalHistory::get(_cur));
		return;
	}

	if (e->key() == Qt::Key_Up)
	{
		_cur++;
		if (_cur >= TerminalHistory::size())
			_cur--;
		document()->setPlainText(TerminalHistory::get(_cur));
		return;
	}



	if (e->key() == Qt::Key_C && e->modifiers() == Qt::ControlModifier)
	{
		clear();
		moveCursor(QTextCursor::Start);
		cancel();
	}

	if (e->key() == Qt::Key_Pause)
	{
		clear();
		moveCursor(QTextCursor::Start);
		pauseBreak();
	}



	QTextEdit::keyPressEvent(e);
}