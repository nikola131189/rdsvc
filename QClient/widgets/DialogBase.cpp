#include "DialogBase.h"

DialogBase::DialogBase(const QString& title, QWidget *parent, bool isGlassClickable)
	: QWidget(parent)
{
	ui.setupUi(this);
	QWidget *_root = this;
	do {
		_root = (QWidget*)_root->parent();
	} while (_root->parent());

	QWidget *_title = new QWidget(this);
	_title->setMinimumHeight(50);
	QVBoxLayout* l = new QVBoxLayout(this);
	l->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
	_title->setLayout(l);

	l->addWidget(new QLabel(title, this));


	_glass = new GlassDialogContainer(_title, this, _root, isGlassClickable);

}

DialogBase::~DialogBase()
{
}

void DialogBase::show()
{
	_glass->show();
}

void DialogBase::hide()
{
	_glass->hide();

}
