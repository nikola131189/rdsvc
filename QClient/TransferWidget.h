#pragma once

#include <QWidget>
#include "ui_TransferWidget.h"
#include "widgets/TabItem.h"
#include <rd/rd.h>

#include "widgets/ItemDelegate.h"
#include "widgets/TreeView.h"
#include "widgets/TreeModel.h"
#include <QHBoxLayout>
#include <QTimer>
#include <QSettings>
#include <QKeyEvent>
#include "FileTransfer.h"

struct TransferModel;
class TransferProxyModel;
struct TransferDelegate;


class TransferWidget : public TabItem
{
	Q_OBJECT

public:
	TransferWidget(TransferModel* model, QWidget *parent = Q_NULLPTR);
	~TransferWidget();
	QByteArray getState();
	void setState(const QByteArray& s);
private:
	TreeView* _tree;
	TransferModel* _model;
	TransferProxyModel* _proxyModel;
	bool eventFilter(QObject* watched, QEvent* event);
	void readSettings();	
	void tick();
private:
	Ui::TransferWidget ui;
};





struct TransferModel : public TreeModel, public TransferHandler
{
	Q_OBJECT
public:
	TransferModel(QObject* p = nullptr);
	~TransferModel();
	void update(double dt);
	void remove(uint32_t id);
	void post(const Rd::Client& cl, const std::filesystem::path& p1, const std::filesystem::path& p2);
	void get(const Rd::Client& cl, const std::filesystem::path& p1, const std::filesystem::path& p2);
signals:
	void transferEvent(const Rd::TransferEvent& ev);
public slots:
	void onTransferEvent(const Rd::TransferEvent& ev);
private:

	void tick();
	void onSuccess(TransferPtr ptr);
	void onError(TransferPtr ptr);
private:
	std::map<uint32_t, TransferPtr> _transfers, _successed, _current;
	uint32_t _id = 0;
	file_system::FileBuffer _fileBuffer;
	const size_t _maxCurrent = 3;
	
};


#include <QSortFilterProxyModel>

class TransferProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	TransferProxyModel(QObject* parent = 0) : QSortFilterProxyModel(parent) {}

protected:
	//bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
	bool lessThan(const QModelIndex& left, const QModelIndex& right) const override
	{
		auto l = ((TreeItem*)left.internalPointer())->data<TransferPtr>();
		auto r = ((TreeItem*)right.internalPointer())->data<TransferPtr>();
		switch (left.column())
		{
		case 0: return l->client().name < r->client().name;
		case 1: return std::wstring(l->path1().c_str()) < std::wstring(r->path1().c_str());
		case 2: return std::wstring(l->path2().c_str()) < std::wstring(r->path2().c_str());
		case 3: return l->offset() < r->offset();
		case 4: return l->id() < r->id();
		case 5: return l->speed() < r->speed();
		case 6: return l->lastActive() < r->lastActive();
		case 7: return l->state() < r->state();
		case 8: return l->ec().message < r->ec().message;
		case 9: return l->ec().value < r->ec().value;
		}
		return true;
	}
};

#include <QPainter>

class TransferDelegate : public ItemDelegate
{
	Q_OBJECT
public:
	TransferDelegate(QObject* parent = 0) : ItemDelegate(parent) {}
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
	void paintClient(QPainter* painter, const QRect& rect, const QModelIndex& index) const;
	std::string state_to_str(AbstractTransfer::State s) const;

};
