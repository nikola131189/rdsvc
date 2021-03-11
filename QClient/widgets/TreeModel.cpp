#include "TreeModel.h"


TreeItem::TreeItem(ItemPtr p, const std::any& v) : _data(v), _parent(p)
{}

TreeItem::TreeItem(ItemPtr p) : _parent(p)
{}

TreeItem::TreeItem() : _parent(nullptr)
{}



ItemPtr TreeItem::parent() { return _parent; }


ItemPtr TreeItem::get(int n) { return _childs[n]; }

int TreeItem::row()
{
	if (!_parent) return 0;
	int k = 0;
	for (auto it = _parent->_childs.begin(); it != _parent->_childs.end(); it++)
	{
		if ((*it) == shared_from_this()) return k;
		++k;
	}
	return 0;
}


int TreeItem::size() { return _childs.size(); }


void TreeItem::disconnect()
{
	if (!_parent) return;
	for (auto it = _parent->_childs.begin(); it != _parent->_childs.end(); it++)
	{
		if ((*it) == shared_from_this())
		{
			_parent->_childs.erase(it);
			return;
		}
	}
}


void TreeItem::clear() 
{
	_childs.clear(); 
}



const std::type_info& TreeItem::dataType() const
{
	return _data.type();
}










TreeModel::TreeModel(QObject* p) : QAbstractItemModel(p)
{
	_root = std::make_shared<TreeItem>();
}


TreeModel::~TreeModel()
{
	_root->clear();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex& parent) const
{

	if (!hasIndex(row, column, parent))
		return QModelIndex();

	TreeItem* parentItem;

	if (!parent.isValid())
		parentItem = _root.get();
	else
		parentItem = static_cast<TreeItem*>(parent.internalPointer());

	ItemPtr childItem = parentItem->get(row);
	if (childItem)
		return createIndex(row, column, childItem.get());
	else
		return QModelIndex();
}


int TreeModel::columnCount(const QModelIndex& parent) const
{
	return _colTitles.size();
}



QModelIndex TreeModel::parent(const QModelIndex& index) const
{
	if (!index.isValid())
		return QModelIndex();

	TreeItem* childItem = static_cast<TreeItem*>(index.internalPointer());
	auto parentItem = childItem->parent();

	if (parentItem == _root)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem.get());
}


int TreeModel::rowCount(const QModelIndex& parent) const
{
	TreeItem* parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = _root.get();
	else
		parentItem = static_cast<TreeItem*>(parent.internalPointer());

	return parentItem->size();
}


QVariant TreeModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole)
		return QVariant();


	return 0;
	//return item;
}


QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole || _colTitles.size() <= section)
		return QVariant();
	return _colTitles[section].c_str();
}




void TreeModel::setColumnTitles(const std::vector<std::string>& arg)
{
	_colTitles = arg;
}



Qt::DropActions TreeModel::supportedDropActions() const
{
	return Qt::CopyAction;
}


ItemPtr TreeModel::root()
{
	return _root;
}



Qt::ItemFlags TreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return 0;

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | QAbstractItemModel::flags(index);
	//return QAbstractItemModel::flags(index);
}
