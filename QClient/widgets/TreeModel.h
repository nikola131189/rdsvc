#pragma once
#include <QAbstractItemModel>
#include <QString>
#include <QMimeData>
#include <QObject>
#include <QUrl>
#include <vector>
#include <mutex>
#include <QList>
#include <functional>
#include <QDebug>
#include <any>
#include <iterator>

class TreeItem;

typedef std::shared_ptr<TreeItem> ItemPtr;

class TreeItem : public std::enable_shared_from_this<TreeItem>
{
public:
	typedef typename std::vector<ItemPtr>::iterator itr;
public:
	TreeItem(ItemPtr p, const std::any& v);
	TreeItem(ItemPtr p);
	TreeItem();
	~TreeItem() {
		clear();
	}

	template<typename T>
	T data() { return std::any_cast<T>(_data); }

	template<typename T>
	void setData(T&& t)
	{
		_data = t;
	}
	ItemPtr parent();
	ItemPtr get(int n);
	int row();
	int size();
	void disconnect();
	void clear();

	template<typename T>
	void emplaceBack(T&& t)
	{
		_childs.emplace_back(std::make_shared<TreeItem>(shared_from_this(), t));
	}
	const std::type_info& dataType() const;

	template<typename T>
	void insert(int pos, T&& t)
	{
		_childs.insert(_childs.begin() + pos, std::make_shared<TreeItem>(shared_from_this(), t));
	}



private:
	std::vector<ItemPtr> _childs;
	std::any _data;
	ItemPtr _parent;
};









class TreeModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	TreeModel(QObject* p = nullptr);
	~TreeModel();
	QModelIndex index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent) const;
	QModelIndex parent(const QModelIndex& index) const;
	int rowCount(const QModelIndex& parent) const;
	QVariant data(const QModelIndex& index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	void setColumnTitles(const std::vector<std::string>& arg);
	Qt::DropActions supportedDropActions() const;
	ItemPtr root();
	Qt::ItemFlags flags(const QModelIndex& index) const;
private:
	ItemPtr _root;
	std::vector<std::string> _colTitles;
};






