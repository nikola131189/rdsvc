#include "ScrollContainer.h"

ScrollContainer::ScrollContainer(QWidget *parent)
	: QScrollArea(parent)
{
	ui.setupUi(this);
	this->horizontalScrollBar()->hide();
}

ScrollContainer::~ScrollContainer()
{
}

void ScrollContainer::clear()
{
	_widgets.clear();
	drawWidgets();
}

void ScrollContainer::addWidget(QWidget* widget)
{
	_widgets.push_back(widget);
	drawWidgets();
}

void ScrollContainer::removeWidget(QWidget* w)
{
	_widgets.erase(std::find(std::begin(_widgets), std::end(_widgets), w));
	drawWidgets();
}

void ScrollContainer::resizeEvent(QResizeEvent* ev)
{
	drawWidgets();
}

void ScrollContainer::drawWidgets()
{
	int h = 0, w = 0;
	QVBoxLayout *_layout = new QVBoxLayout(this);
	for (auto it : _widgets)
	{
		_layout->addWidget(it);
		h += it->height() + _layout->margin();
	}
	w = width();
	if (h > height())
		w -= verticalScrollBar()->width();

	QWidget* wid = new QWidget(this);

	wid->setLayout(_layout);
	wid->setMinimumWidth(w);
	setWidget(wid);
}
