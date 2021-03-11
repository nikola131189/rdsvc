#pragma once

#include <QTreeView>
#include "ui_TreeView.h"
#include <QScrollBar>
class TreeView : public QTreeView
{
	Q_OBJECT

public:
	TreeView(QWidget *parent = Q_NULLPTR);
	~TreeView();
private:
	Ui::TreeView ui;
};
