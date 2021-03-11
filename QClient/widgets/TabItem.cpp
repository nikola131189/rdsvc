#include "TabItem.h"

TabItem::TabItem(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

TabItem::~TabItem()
{
}

void TabItem::paintEvent(QPaintEvent* event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
