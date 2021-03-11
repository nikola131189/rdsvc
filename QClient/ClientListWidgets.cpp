#include "ClientListWidgets.h"

ClientListWidgets::ClientListWidgets(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	_tree = new TreeView(this);

	_filterEdit = new LineEdit(this);



	_model = new ClientsListModel(this);

	_delegate = new ClientsListDelegate(this);

	_proxyModel = new ClientsListProxyModel(this);
	//_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

	_model->setColumnTitles({ "Name", "Ping", "Address", "Connection time" });

	_proxyModel->setSourceModel(_model);

	_tree->setSortingEnabled(true);
	_tree->setRootIsDecorated(false);
	_tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
	//_tree->setSelectionBehavior(QAbstractItemView::selectA);
	_tree->setModel(_proxyModel);
	_tree->setItemDelegate(_delegate);


	QObject::connect(_filterEdit, &LineEdit::textChanged, [this](const QString& str) {
		_tree->selectionModel()->clearCurrentIndex();
		_proxyModel->setFilterRegularExpression(str);
	});

	QObject::connect(_model, &QAbstractItemModel::dataChanged, [this]() {
		_proxyModel->invalidate();
	});


	QObject::connect(_model, &QAbstractItemModel::modelAboutToBeReset, [this]() {
		_selected.clear();
		auto ind = _tree->selectionModel()->selectedIndexes();
		if (ind.isEmpty())
			return;
		for (auto& index : ind)
		{
			if (index.isValid())
			{
				index = _proxyModel->mapToSource(index);
				auto it = (TreeItem*)index.internalPointer();
				_selected.push_back(it->data<Rd::Client>());
			}
		}

	});


	QObject::connect(_model, &QAbstractItemModel::modelReset, [this]() {

		for (auto & client : _selected)
		{
			auto ind = _model->find(client.id);
			if (ind.isValid())
			{
				_tree->selectionModel()->blockSignals(true);
				_tree->selectionModel()->select(_proxyModel->mapFromSource(ind), QItemSelectionModel::SelectCurrent);
				_delegate->selectRow(ind.row());
				_tree->selectionModel()->blockSignals(false);
			}
		}
	});




	QObject::connect(_tree->selectionModel(), &QItemSelectionModel::selectionChanged, [this](const QItemSelection& selected, const QItemSelection& deselected) {
		_isSelectionChanged = true;
		if (selected.indexes().isEmpty())
		{
			clientChanged({});
		}
		else
		{
			auto ind = _proxyModel->mapToSource(selected.indexes()[0]);
			if (ind.isValid())
			{
				clientChanged(((TreeItem*)ind.internalPointer())->data<Rd::Client>());
				_delegate->selectRow(ind.row());
				_proxyModel->invalidate();
			}
		}
	});


	QObject::connect(_tree, &QTreeView::clicked, [&](const QModelIndex& index) {
		if (_isSelectionChanged)
		{
			_isSelectionChanged = false;
			return;
		}
		auto ind = _proxyModel->mapToSource(index);
		if (ind.isValid())
		{
			clientChanged(((TreeItem*)ind.internalPointer())->data<Rd::Client>());
			_delegate->selectRow(ind.row());
			_proxyModel->invalidate();
		}
	});


	

	QVBoxLayout* l = new QVBoxLayout(this);
	l->addWidget(_filterEdit);
	l->addWidget(_tree);
	l->setMargin(0);
	setLayout(l);



}

ClientListWidgets::~ClientListWidgets()
{
}


QByteArray ClientListWidgets::getState()
{
	return _tree->header()->saveState();
}

void ClientListWidgets::setState(const QByteArray& s)
{
	_tree->header()->restoreState(s);
}




ClientsListModel::ClientsListModel(QObject* p)
	: TreeModel(p)
{
	QObject::connect(this, &ClientsListModel::clientsEvent, this, &ClientsListModel::onClientsEvent);

	QTimer* timer = new QTimer(this);
	timer->setInterval(1000);

	QObject::connect(timer, &QTimer::timeout, [&] {
		Rd::Inet::send(0, Rd::ClientsRequestEvent());
		if (!_updated) clear();
		_updated = false;
		});
	timer->start();

	EventBus::subscribe<Rd::ClientsEvent>([&](const Rd::ClientsEvent& e) {
		clientsEvent(e);
	});


}


QModelIndex ClientsListModel::find(uint32_t id)
{
	for (int i = 0; i < root()->size(); i++)
	{
		if (id == root()->get(i)->data<Rd::Client>().id)
		{
			return index(i);
		}
	}
	return QModelIndex();
}



void ClientsListModel::clear()
{
	/*beginRemoveRows(QModelIndex(), 0, root()->size());
	root()->clear();
	endRemoveRows();*/
	beginResetModel();
	root()->clear();
	endResetModel();
	//dataChanged(index(0), index(0));
}

void ClientsListModel::onClientsEvent(const Rd::ClientsEvent& ev)
{

	_updated = true;
	/*if (ev.clients.size() > root()->size())
	{
		for (size_t i = 0; i < tmp.size(); i++)
		{
			if (i >= root()->size())
			{
				beginInsertRows(QModelIndex(), i, i);
				root()->insert(i, tmp[i]);
				endInsertRows();
			}

			if (tmp[i].id != root()->get(i)->data<Rd::Client>().id)
			{
				beginInsertRows(QModelIndex(), i, i);
				root()->insert(i, tmp[i]);
				endInsertRows();
			}
		}
	}


	if (ev.clients.size() < root()->size())
	{
		for (size_t i = 0; i < tmp.size(); i++)
		{
			if (tmp[i].id != root()->get(i)->data<Rd::Client>().id)
			{
				beginRemoveRows(QModelIndex(), i, i);
				root()->get(i)->disconnect();
				endRemoveRows();
			}
		}
	}
	*/

	if (ev.clients.size() == root()->size())
	{
		for (size_t i = 0; i < ev.clients.size(); i++)
		{
			root()->get(i)->setData(ev.clients[i]);
		}
		dataChanged(index(0), index(ev.clients.size()));
	}
	else
	{
		beginResetModel();
		root()->clear();
		for (size_t i = 0; i < ev.clients.size(); i++)
		{
			root()->emplaceBack(ev.clients[i]);
		}
		endResetModel();
	}

}


















ClientsListDelegate::ClientsListDelegate(QObject* parent) : ItemDelegate(parent)
{

}


void ClientsListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{


	//painter->save();
	QRect r = option.rect.adjusted(0, 0, 0, 0);

	if (option.state & QStyle::State_MouseOver)
	{
		auto c = option.palette.highlight().color();
		float v = 0.6f;
		painter->fillRect(r, QColor(c.red() * v, c.green() * v, c.blue() * v));
	}


	QSortFilterProxyModel* pm = reinterpret_cast<QSortFilterProxyModel*>((void*)(index.model()));
	QModelIndex ind = pm->mapToSource(index);
	if (!ind.isValid())
		return;

	ClientsListModel* model = (ClientsListModel*)pm->sourceModel();

	auto it = (TreeItem*)ind.internalPointer();

	
	if (option.state & QStyle::State_Selected || _selRow == ind.row())
	{
		painter->fillRect(r, option.palette.highlight());
	}



	/*else if (index.row() == 0)
	{
		_widget->setVisible(false);
	}*/



	paintClient(painter, option, ind);

	//painter->restore();

}



QSize ClientsListDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return QStyledItemDelegate::sizeHint(option, index);
}



void ClientsListDelegate::paintClient(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (!index.isValid())
		return;


	QRect r;
	r = option.rect.adjusted(5, 0, -5, 0);

	auto it = (TreeItem*)index.internalPointer();
	Rd::Client client = it->data<Rd::Client>();

	switch (index.column())
	{
	case 0:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft | Qt::AlignVCenter, client.name.c_str());
		break;
	case 1:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignHCenter | Qt::AlignVCenter, std::to_string(client.ping).c_str());
		break;
	case 2:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft | Qt::AlignVCenter, addressV4ToStr(client.addressV4).c_str());
		break;
	case 3:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft | Qt::AlignVCenter, dateToStr(client.connectionTime).c_str());
		break;
	default:
		break;
	}
}

void ClientsListDelegate::selectRow(int row) const
{
	_selRow = row;
}
