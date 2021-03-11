#include "TabWidget.h"


TabBar::TabBar(QWidget* parent ) : QTabBar(parent)
{
	//setStyleSheet(" background-color: #1E1E1E");
	setDrawBase(false);
}


void TabBar::pin(int const index)
{
	this->setTabButton(index, QTabBar::RightSide, 0);
	PinnedTab info;
	info.isPinned = true;
	info.currentIndex = index; // TODO: move the tab to the left and do not use current index
	setTabData(index, QVariant::fromValue(info));
}


void TabBar::tabLayoutChange() 
{
	for (int i = 0; i != count(); ++i) // Check if a pinned tab has moved
	{
		if (tabData(i).isValid())
		{
			PinnedTab const info = tabData(i).value<PinnedTab>();
			if (info.isPinned == true && i != info.currentIndex) {
				rollbackLayout();
				return;
			}
		}
	}

	for (int i = 0; i != count(); ++i)
	{
		if (tabData(i).isValid())
		{
			PinnedTab info = tabData(i).value<PinnedTab>();
			info.currentIndex = i;
			setTabData(i, QVariant::fromValue(info));
		}
		else
		{
			PinnedTab info;
			info.isPinned = false;
			info.currentIndex = i;
			setTabData(i, QVariant::fromValue(info));
		}
	}
}

void TabBar::rollbackLayout() {
	for (int i = 0; i != count(); ++i)
	{
		if (tabData(i).isValid())
		{
			PinnedTab const info = tabData(i).value<PinnedTab>();
			if (i != info.currentIndex) {
				moveTab(i, info.currentIndex);
			}
		}
	}
}




void TabBar::mouseReleaseEvent(QMouseEvent* ev) 
{
	int n = tabAt(ev->pos());
	PinnedTab const info = tabData(n).value<PinnedTab>();

	if (ev->button() == Qt::MiddleButton)
	{

		if (n >= 0 && !info.isPinned)
		{
			tabCloseRequested(n);
			return;
		}
	}
	QTabBar::mouseReleaseEvent(ev);
}

















TabWidget::TabWidget(QWidget* parent)
	: QTabWidget(parent)
{
	_tabBar = new TabBar(this);
	setTabBar(_tabBar);
	//tabBar()->installEventFilter(this);
	//setStyleSheet(" background-color: #1E1E1E");
}

TabWidget::~TabWidget()
{
}

void TabWidget::pin(int index)
{
	_tabBar->pin(index);

}



void TabWidget::paintEvent(QPaintEvent* ev)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void TabWidget::keyPressEvent(QKeyEvent* ev)
{
	if (ev->key() == Qt::Key_W && ev->modifiers() == Qt::ControlModifier)
	{
		int n = currentIndex();
		PinnedTab const info = tabBar()->tabData(n).value<PinnedTab>();
		if (!info.isPinned)
		{
			tabCloseRequested(n);
		}
	}
	QTabWidget::keyPressEvent(ev);
}

