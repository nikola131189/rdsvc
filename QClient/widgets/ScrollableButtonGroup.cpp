#include "ScrollableButtonGroup.h"

ScrollableButtonGroup::ScrollableButtonGroup(const QString& title, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	_container = new ScrollContainer(this);

	QLabel* label = new QLabel(title, this);
	label->setAlignment(Qt::AlignCenter);
	QVBoxLayout* l = new QVBoxLayout;
	l->setMargin(0);
	l->addWidget(label);
	l->addWidget(_container);
	
	setLayout(l);


	_buttonGroup = new QButtonGroup(this);
	QObject::connect(_buttonGroup, &QButtonGroup::idClicked, [this](int id) { this->idClicked(id); });
}

ScrollableButtonGroup::~ScrollableButtonGroup()
{
}

void ScrollableButtonGroup::addButton(QAbstractButton* btn, int id)
{
	_buttonGroup->addButton(btn, id);
	_container->addWidget(btn);
}

void ScrollableButtonGroup::removeButton(QAbstractButton* btn)
{
	_buttonGroup->removeButton(btn);
	_container->removeWidget(btn);
}

void ScrollableButtonGroup::clear()
{
	auto buttons = _buttonGroup->buttons();
	for (auto& b : buttons)
		_buttonGroup->removeButton(b);
	_container->clear();
}
