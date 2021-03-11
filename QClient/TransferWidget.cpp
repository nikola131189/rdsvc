#include "TransferWidget.h"

TransferWidget::TransferWidget(TransferModel* model, QWidget *parent)
	: TabItem(parent)
{
	ui.setupUi(this); 
	_model = model;
	_proxyModel = new TransferProxyModel(this);
	_proxyModel->setSourceModel(_model);

	_model->setColumnTitles({ "client", "path1", "path2", "offset", "id", "speed", "last_active", "state", "error message", "error code" });


	_tree = new TreeView(this);
	_tree->setSortingEnabled(true);
	_tree->sortByColumn(6, Qt::SortOrder::DescendingOrder);
	_tree->setRootIsDecorated(false);
	_tree->setModel(_proxyModel);
	_tree->setItemDelegate(new TransferDelegate(this));
	_tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
	_tree->header()->resizeSection(0, 80);
	_tree->header()->resizeSection(1, 200);
	_tree->header()->resizeSection(2, 200);

	//_tree->setFocusPolicy(Qt::ClickFocus);

	QHBoxLayout* l = new QHBoxLayout(this);
	l->addWidget(_tree);
	l->setMargin(0);
	setLayout(l);

	readSettings();


	QTimer* timer = new QTimer(this);
	timer->setInterval(500);

	QObject::connect(timer, &QTimer::timeout, [&] {
		_model->update(500);
		_proxyModel->invalidate();
	});
	timer->start();

	installEventFilter(this);

}

TransferWidget::~TransferWidget()
{
	/*QSettings settings("sett.ini", QSettings::IniFormat);
	settings.setValue("TransferWidget/tree/state", _tree->header()->saveState());*/
}

QByteArray TransferWidget::getState()
{
	return _tree->header()->saveState();
}

void TransferWidget::setState(const QByteArray& s)
{
	_tree->header()->restoreState(s);
}

void TransferWidget::readSettings()
{
	/*QSettings settings("sett.ini", QSettings::IniFormat);
	bool res = _tree->header()->restoreState(settings.value("TransferWidget/tree/state").toByteArray());*/
}




bool TransferWidget::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent* ev = (QKeyEvent*)event;

		if (ev->key() == Qt::Key_Delete)
		{
			QModelIndexList list = _tree->selectionModel()->selectedIndexes();
			if (list.isEmpty()) return 0;
			for (auto it = list.begin(); it != list.end(); it++)
			{
				auto ind = _proxyModel->mapToSource(*it);
				TreeItem* i = (TreeItem*)ind.internalPointer();
				if (ind.column() == 0)
				{
					_model->remove(i->data<TransferPtr>()->id());
				}
			}
		}
	}

	return 0;
}













TransferModel::TransferModel( QObject* p)
	: TreeModel(p)
{
	QObject::connect(this, &TransferModel::transferEvent, this, &TransferModel::onTransferEvent);

	QTimer* timer = new QTimer(this);
	timer->setInterval(2000);

	QObject::connect(timer, &QTimer::timeout, [&] {
		tick();
	});
	timer->start();


	EventBus::subscribe<Rd::TransferEvent>([&](const Rd::TransferEvent& ev) {
		transferEvent(ev);
	});
}

TransferModel::~TransferModel()
{
}

void TransferModel::update(double dt)
{

	if (_transfers.size() + _successed.size() + _current.size() != root()->size())
	{
		beginResetModel();
		root()->clear();
		for (auto& it : _transfers)
		{
			root()->emplaceBack(it.second);
		}

		for (auto& it : _successed)
		{
			root()->emplaceBack(it.second);
		}

		for (auto& it : _current)
		{
			root()->emplaceBack(it.second);
		}
		endResetModel();
	}


	for (size_t i = 0; i < root()->size(); i++)
	{
		root()->get(i)->data<TransferPtr>()->snapshot(dt);
	}
}

void TransferModel::remove(uint32_t id)
{
	_current.erase(id);
	_successed.erase(id);
	_transfers.erase(id);
}

void TransferModel::onTransferEvent(const Rd::TransferEvent& ev)
{
	{
		auto it = _current.find(ev.id);
		if (it != _current.end())
			it->second->onTransfer(ev);
	}
}


void TransferModel::post(const Rd::Client& cl, const std::filesystem::path& p1, const std::filesystem::path& p2)
{
	auto t = std::make_shared<PostTransfer>(cl, _id, p1, p2, _fileBuffer, *this);
	
	if (_current.size() < _maxCurrent)
	{
		_current[_id] = t;
		t->start();
	}
	else
	{
		_transfers[_id] = t;
	}

	_id++;
}

void TransferModel::get(const Rd::Client& cl, const std::filesystem::path& p1, const std::filesystem::path& p2)
{
	auto t = std::make_shared<GetTransfer>(cl, _id, p1, p2, _fileBuffer, *this);
	if (_current.size() < _maxCurrent)
	{
		_current[_id] = t;
		t->start();
	}
	else
	{
		_transfers[_id] = t;
	}

	_id++;
}


void TransferModel::tick()
{
	if (!_current.empty())
	{
		for (auto& it : _current)
		{
			AbstractTransfer::State s = it.second->state();

			if (s == AbstractTransfer::running || s == AbstractTransfer::error)
			{
				if (utility::get_tick_count() - it.second->lastActive() > 5000)
				{
					it.second->start();
				}
			}
		}
	}
}


void TransferModel::onSuccess(TransferPtr ptr)
{
	_current.erase(ptr->id());

	_successed[ptr->id()] = ptr;
	if (!_transfers.empty())
	{
		auto t = _transfers.begin()->second;
		_current[t->id()] = t;
		t->start();
		_transfers.erase(t->id());
	}
}

void TransferModel::onError(TransferPtr ptr)
{

}




























void TransferDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QSortFilterProxyModel* pm = reinterpret_cast<QSortFilterProxyModel*>((void*)(index.model()));
	QModelIndex ind = pm->mapToSource(index);

	painter->save();

	if (option.state & QStyle::State_MouseOver)
	{
		auto c = option.palette.highlight().color();
		float v = 0.6f;
		painter->fillRect(option.rect, QColor(c.red() * v, c.green() * v, c.blue() * v));
	}


	if (option.state & QStyle::State_Selected)
	{
		QRect r = option.rect.adjusted(0, 0, 0, 0);

		//r.setWidth(option.widget->width());
		//r.setX(0);
		painter->fillRect(r, option.palette.highlight());
	}

	paintClient(painter, option.rect, ind);
	painter->restore();
}

QSize TransferDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return QStyledItemDelegate::sizeHint(option, index);
}
void TransferDelegate::paintClient(QPainter* painter, const QRect& rect, const QModelIndex& index) const
{

	QRect r;
	std::stringstream s;
	s << index.row() + 1;
	r = rect.adjusted(5, 0, -5, 0);


	auto it = ((TreeItem*)index.internalPointer())->data<TransferPtr>();

	switch (index.column())
	{
	case 0:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft | Qt::AlignVCenter, QString::fromUtf8(it->client().name.c_str()));
		break;

	case 1:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft | Qt::AlignVCenter, QString::fromUtf8((char *)it->path1().generic_u8string().c_str()));
		break;
	case 2:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft | Qt::AlignVCenter, QString::fromUtf8((char *)it->path2().generic_u8string().c_str()));
		break;
	case 3:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignHCenter | Qt::AlignVCenter, sizeToStr(it->offset()).c_str());
		break;
	case 4:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignHCenter | Qt::AlignVCenter, std::to_string(it->id()).c_str());
		break;
	case 5:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignHCenter | Qt::AlignVCenter, std::to_string(it->speed()).c_str());
		break;
	case 6:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignHCenter | Qt::AlignVCenter, std::to_string(it->lastActive()).c_str());
		break;
	case 7:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignHCenter | Qt::AlignVCenter, state_to_str(it->state()).c_str());
		break;
	case 8:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft | Qt::AlignVCenter, QString::fromUtf8(it->ec().message.c_str()));
		break;
	case 9:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignHCenter | Qt::AlignVCenter, std::to_string(it->ec().value).c_str());
		break;
	default:
		break;
	}
}



std::string TransferDelegate::state_to_str(AbstractTransfer::State s) const
{
	switch (s)
	{
	case AbstractTransfer::running: return "running";
	case AbstractTransfer::stopped: return "stopped";
	case AbstractTransfer::ready: return "ready";
	case AbstractTransfer::error: return "error";
	case AbstractTransfer::success: return "success";
	default: return "";
	}
}


