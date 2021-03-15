#include "InputDialogWidget.h"

InputDialogWidget::InputDialogWidget(const QString& title, const QString& text, QWidget *parent)
	: AbstractDialogWidget(parent)
{
	ui.setupUi(this);
	_edit = new LineEdit(this);
	_edit->setText(text);

	QLabel* label = new QLabel(title, this);
	label->setAlignment(Qt::AlignCenter);

	QHBoxLayout* l1 = new QHBoxLayout(this);
	QWidget* w1 = new QWidget(this);
	w1->setLayout(l1);

	QPushButton* ok = new QPushButton("ok", this);
	QObject::connect(ok, &QPushButton::clicked, [this]() {okPressed(); });
	l1->addWidget(ok);

	QPushButton* cancel = new QPushButton("cancel", this);
	QObject::connect(cancel, &QPushButton::clicked, [this]() {cancelPressed(); });
	l1->addWidget(cancel);

	QVBoxLayout* l = new QVBoxLayout(this);
	l->addWidget(label);
	l->addWidget(_edit);
	l->addWidget(w1);
	setLayout(l);

	setMinimumWidth(500);
	setMaximumHeight(200);

	_edit->installEventFilter(this);
	installEventFilter(this);
}

InputDialogWidget::~InputDialogWidget()
{
}

QString InputDialogWidget::text()
{
	return _edit->text();
}


void InputDialogWidget::focusInEvent(QFocusEvent* event)
{
	_edit->setFocus();
}

bool InputDialogWidget::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent* e = (QKeyEvent*)event;
		if (e->key() == Qt::Key_Escape)
			cancelPressed();
		if (e->key() == Qt::Key_Return)
			okPressed();
	}

	return false;
}
