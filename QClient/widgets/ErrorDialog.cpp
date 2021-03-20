#include "ErrorDialog.h"

ErrorDialog::ErrorDialog(const QString& title, QWidget *parent)
	: DialogBase(title, parent)
{
	ui.setupUi(this);

	_edit = new QPlainTextEdit(this);
	_btn = new QPushButton("ok", this);
	_btn->setDefault(true);
	_checkBox = new QCheckBox("Show this message", this);
	_checkBox->setChecked(true);
	QObject::connect(_btn, &QPushButton::clicked, [this]() { hide(); });
	QVBoxLayout* l = new QVBoxLayout(this);
	l->addWidget(_checkBox);
	l->addWidget(_edit);
	l->addWidget(_btn);
	setLayout(l);
	setMaximumHeight(200);

	installEventFilter(this);
	_edit->installEventFilter(this);
}

ErrorDialog::~ErrorDialog()
{
}

void ErrorDialog::showMessage(const QString& mess)
{
	_edit->clear();
	_edit->insertPlainText(mess);
	if(_checkBox->isChecked())
		show();
}


bool ErrorDialog::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent* e = (QKeyEvent*)event;
		if (e->key() == Qt::Key_Escape)
			hide();
		if (e->key() == Qt::Key_Return)
			hide();
	}
	return QObject::eventFilter(watched, event);
}

void ErrorDialog::showEvent(QShowEvent* event)
{
	_btn->setFocus();
}
