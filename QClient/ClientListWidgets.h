#pragma once

#include <QWidget>
#include "ui_ClientListWidgets.h"

#include <rd/rd.h>
#include "widgets/TreeView.h"
#include "widgets/TreeModel.h"
#include "widgets/LineEdit.h"
#include <QTimer>
#include <QVBoxLayout>


class ClientsListModel;
class ClientsListProxyModel;
class ClientsListDelegate;


class ClientListWidgets : public QWidget
{
	Q_OBJECT

public:
	ClientListWidgets(QWidget *parent = Q_NULLPTR);
	~ClientListWidgets();
	QByteArray getState();
	void setState(const QByteArray& s);
signals:
	void clientChanged(const Rd::Client& cl);
private:
	Ui::ClientListWidgets ui;


	LineEdit* _filterEdit;

	TreeView* _tree;
	ClientsListModel* _model;
	ClientsListProxyModel* _proxyModel;
	ClientsListDelegate* _delegate;
	std::list<Rd::Client> _selected;

	bool _isSelectionChanged = false;
};






class ClientsListModel :
	public TreeModel
{
	Q_OBJECT

public:
	ClientsListModel(QObject* p = nullptr);

	QModelIndex find(uint32_t id);

	void clear();
signals:
	void clientsEvent(const Rd::ClientsEvent& ev);
public slots: 
	void onClientsEvent(const Rd::ClientsEvent& ev);
private:
	bool _updated = false;

};





#include <QSortFilterProxyModel>


class ClientsListProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	ClientsListProxyModel(QObject* parent = 0) : QSortFilterProxyModel(parent) {}

protected:
	bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override
	{
		auto index = this->sourceModel()->index(source_row, 0);
		if (!index.isValid())
			return 0;
		auto cl = ((TreeItem*)index.internalPointer())->data<Rd::Client>();
		auto re = this->filterRegularExpression();
		re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
		return re.match(cl.name.c_str()).hasMatch();
	}


	bool lessThan(const QModelIndex& left, const QModelIndex& right) const override
	{
		if (!left.isValid() || !right.isValid())
			return 0;

		auto l = ((TreeItem*)left.internalPointer())->data<Rd::Client>();
		auto r = ((TreeItem*)right.internalPointer())->data<Rd::Client>();
		switch (left.column())
		{
		case 0: return l.name < r.name;
		case 1: return l.ping < r.ping;
		case 2: return l.addressV4 < r.addressV4;
		case 3: return l.connectionTime < r.connectionTime;
		}
		return true;
	}
};


















#include "widgets/ItemDelegate.h"
#include <QPainter>

class ClientsListDelegate : public ItemDelegate
{
	Q_OBJECT
public:
	ClientsListDelegate(QObject* parent = 0);
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
	void paintClient(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	void selectRow(int row) const;
private: 
	mutable int _selRow = -1;
};

