#include "TreeView.h"

TreeView::TreeView(QWidget *parent)
	: QTreeView(parent)
{
	ui.setupUi(this);
	//setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	this->viewport()->setAttribute(Qt::WA_Hover);
}

TreeView::~TreeView()
{
}

