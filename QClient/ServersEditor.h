#pragma once

#include <QWidget>
#include "ui_ServersEditor.h"
#include "ServerInfo.h"
#include <QListView>
#include "widgets/LineEdit.h"
#include <QTextEdit>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QStandardItemModel >
#include "ServersModel.h"
#include "widgets/TreeView.h"
#include <QSplitter>
#include <QLabel>
#include "widgets/AbstractDialogWidget.h"
#include "widgets/LabelLineEdit.h"

class ServerInfoDelegate;

class ServersEditor : public AbstractDialogWidget
{
	Q_OBJECT

public:
	ServersEditor(ServersModel* model, QWidget *parent = Q_NULLPTR);
	~ServersEditor();
	void setServers(std::vector<ServerInfo>& arg);
signals:
	void insert(const ServerInfo& inf);
	void remove(uint64_t id);
private:
	void hideEvent(QHideEvent* event);


	void updateList();
	void setState(const ServerInfo& it);
	ServerInfo getState();


	Ui::ServersEditor ui;
	TreeView* _tree;
	ServersModel* _model;


	LabelLineEdit* _descriptionWidget, * _address1Widget, * _address2Widget, * _imprintWidget,
		* _typeWidget, * _port1Widget, * _port2Widget, *_passwordWidget;
	QTextEdit* _secretWidget;
	QGridLayout* _gridLayout;
	QVBoxLayout* _formLayout;

	ServerInfo _curent;
};



#include "widgets/ItemDelegate.h"
#include <QPainter>

class ServerInfoDelegate : public ItemDelegate
{
	Q_OBJECT
public:
	ServerInfoDelegate(QObject* parent = 0);
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
	void paintClient(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

};