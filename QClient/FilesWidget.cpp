#include "FilesWidget.h"

FilesWidget::FilesWidget(AbstractFilesModel* m, QWidget *parent)
	: _model(m), QWidget(parent)
{
	ui.setupUi(this);

	//_model->setParent(this);
	_proxyModel = new FilesProxyModel(this);
	_proxyModel->setSourceModel(_model);

	_line = new LineEdit(this);

	QObject::connect(_line, &QLineEdit::returnPressed, [&](){
			dir(_line->text().toStdString());
		});


	_tree = new TreeView(this);
	_model->setColumnTitles({ "name", "time", "type", "size", "perms" });
	_tree->setSortingEnabled(true);
	_tree->sortByColumn(0, Qt::AscendingOrder);
	_tree->setRootIsDecorated(false);
	_tree->setModel(_proxyModel);
	_tree->setItemDelegate(new FilesDelegate(this));
	_tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
	_tree->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Interactive);
	_tree->header()->setSectionResizeMode(1, QHeaderView::ResizeMode::Interactive);
	_tree->header()->setSectionResizeMode(2, QHeaderView::ResizeMode::Interactive);
	_tree->header()->setSectionResizeMode(3, QHeaderView::ResizeMode::Interactive);
	_tree->header()->setSectionResizeMode(4, QHeaderView::ResizeMode::Interactive);
	_tree->header()->resizeSection(0, 400);
	_tree->header()->resizeSection(1, 150);
	_tree->header()->resizeSection(3, 150);
	_tree->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(_tree, &TreeView::customContextMenuRequested, [this](const QPoint& p) { _menu->exec(_tree->viewport()->mapToGlobal(p)); });


	QVBoxLayout* l = new QVBoxLayout(this);
	l->setMargin(0);
	QHBoxLayout* hl = new QHBoxLayout(this);

	QPushButton* btn_up = new QPushButton(QIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp)), "", this);
	QObject::connect(btn_up, &QPushButton::clicked, [&]() { up(); });
	hl->addWidget(btn_up);

	hl->addWidget(_line);

	l->addItem(hl);
	l->addWidget(_tree);

	setLayout(l);
	_tree->installEventFilter(this);
	_tree->viewport()->installEventFilter(this);

	_line->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
	btn_up->setFocusPolicy(Qt::NoFocus);


	initMenu();
}

FilesWidget::~FilesWidget()
{
}


void FilesWidget::dir(const std::filesystem::path& p)
{	
	_path = p;
	_model->dir(p);
	_line->setText(QString::fromUtf8((char *)_path.generic_u8string().c_str()));
}


std::filesystem::path FilesWidget::path()
{
	return _path;
}

std::list<files::file_info_t> FilesWidget::selected()
{
	QModelIndexList list = _tree->selectionModel()->selectedIndexes();
	if (list.empty()) 
		return std::list<files::file_info_t>();

	std::list<files::file_info_t> res;
	for (auto it = list.begin(); it != list.end(); it++)
	{
		auto ind = _proxyModel->mapToSource(*it);
		TreeItem* i = (TreeItem*)ind.internalPointer();
		if (ind.column() == 0)
			res.push_back(i->data<files::file_info_t>());
	}
	return res;
}

void FilesWidget::up()
{
	dir(_path.parent_path());
}

QByteArray FilesWidget::getState()
{
	return _tree->header()->saveState();
}

void FilesWidget::setState(const QByteArray& s)
{
	_tree->header()->restoreState(s);
}



bool FilesWidget::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent* ev = (QKeyEvent*)event;
		if (ev->key() == Qt::Key_Backspace)
		{
			up();
		}

		if (ev->key() == Qt::Key_Enter || ev->key() == Qt::Key_Return)
		{

			QModelIndex index = _tree->currentIndex();
			auto ind = _proxyModel->mapToSource(index);
			if (!ind.isValid()) return 0;
			TreeItem* it = (TreeItem*)ind.internalPointer();
			if (it->data<files::file_info_t>().type == files::file_type::directory_file)
				dir(it->data<files::file_info_t>().path);
			else
				openUrl(it->data<files::file_info_t>().path);
		}

		if (ev->key() == Qt::Key_Delete)
		{
			do_remove();
		}

		if (ev->key() == Qt::Key_F2)
		{
			do_rename();
		}

		if (ev->key() == Qt::Key_F6)
		{
			auto l = selected();
			copy(l);
	
		}

		if (ev->key() == Qt::Key_F5)
		{
			dir(_path);
		}

		if (ev->key() == Qt::Key_F7)
		{
			do_mkdir();
		}
		
	}

	if (event->type() == QEvent::FocusIn)
	{
		QModelIndex ind = _tree->currentIndex();
		if (!ind.isValid())
		{
			_tree->selectionModel()->select(_tree->model()->index(0, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
		}
	}

	if (event->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent* ev = (QMouseEvent*)event;
		if (ev->button() == Qt::MouseButton::ExtraButton1)
		{
			up();
		}
	}


	if (event->type() == QEvent::MouseButtonDblClick)
	{
		QMouseEvent* mouseEvent = static_cast <QMouseEvent*> (event);

		if (mouseEvent->button() == Qt::LeftButton) {
			QModelIndex index = _tree->currentIndex();
			if (!index.isValid()) return 0;

			auto ind = _proxyModel->mapToSource(index);

			TreeItem* it = (TreeItem*)ind.internalPointer();


			if (it->data<files::file_info_t>().type == files::file_type::directory_file)
				dir(it->data<files::file_info_t>().path);
			else
				openUrl(it->data<files::file_info_t>().path);
		}


	}

	return 0;
}

void FilesWidget::initMenu()
{
	_menu = new QMenu(this);


	{
		QAction* act = _menu->addAction("update	F5", [this]() {
			dir(_path);
			});
	}

	{
		QAction* act = _menu->addAction("rename	F2", [this]() {
			do_rename();
			});
	}

	{
		QAction* act = _menu->addAction("copy	F6", [this]() {
			auto l = selected();
			copy(l);
		});
	}



	{
		QAction* act = _menu->addAction("mkdir	F7", [this]() {
			do_mkdir();
			});
	}


	{
		QAction* act = _menu->addAction("delete	Del", [this]() {
			do_remove();
			});
	}
}

void FilesWidget::resizeEvent(QResizeEvent* event)
{
/*	if(width() > 500)
		_tree->header()->resizeSection(0, width() - 420);*/
}













void FilesWidget::do_remove()
{
	std::list<files::file_info_t> l = selected();
	for (auto it = l.begin(); it != l.end(); it++)
		_model->remove(it->path);
	QTimer::singleShot(500, [this]() {dir(_path); });
}

void FilesWidget::do_copy()
{
	auto l = selected();
	copy(l);
}

void FilesWidget::do_mkdir()
{
	Gui::Dialog::makeInput("make directory", "new dir", this, [this](QString text, bool ok) {
		if (ok)
		{
			_model->mkdir(_path / text.toStdString());
			QTimer::singleShot(500, [this]() {dir(_path);});
			
		}
		_tree->setFocus();
	});
}

void FilesWidget::do_rename()
{
	std::list<files::file_info_t> l = selected();
	if (l.empty()) return;
	Gui::Dialog::makeInput("rename", QString::fromStdWString(l.front().path.filename()), this, [this, l](QString text, bool ok) {
		if (ok)
		{
			_model->rename(l.front().path, _path / text.toStdString());
			QTimer::singleShot(500, [this]() {dir(_path); });
		}
		_tree->setFocus();
	});
}

















void FilesDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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

	

	if (option.state & QStyle::State_Selected && option.widget->hasFocus())
	{
		QRect r = option.rect.adjusted(0, 0, 0, 0);

		//r.setWidth(option.widget->width());
		//r.setX(0);
		painter->fillRect(r, option.palette.highlight());
	}

	paintClient(painter, option.rect, ind);
	painter->restore();
}

QSize FilesDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return QStyledItemDelegate::sizeHint(option, index);
}
void FilesDelegate::paintClient(QPainter* painter, const QRect& rect, const QModelIndex& index) const
{

	QRect r;
	std::stringstream s;
	s << index.row() + 1;
	r = rect.adjusted(5, 0, -5, 0);



	TreeItem* it = (TreeItem*)index.internalPointer();
	files::file_info_t f = it->data<files::file_info_t>();


	switch (index.column())
	{
	case 0:
		drawIcon(painter, QRect(r.x(), r.y(), rect.height(), rect.height()), f);

		painter->drawText(r.left() + 20, r.top(), r.width() - 20, r.height(), Qt::AlignLeft | Qt::AlignVCenter,
			QString::fromUtf8((char *)f.path.filename().u8string().c_str()));
		break;

	case 2:
		if (f.type == files::file_type::directory_file)
			painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft | Qt::AlignVCenter, "directory");
		else
			painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft | Qt::AlignVCenter, QString::fromUtf8((char *)f.path.extension().u8string().c_str()));
		break;

	case 3:
		if (f.type != files::file_type::directory_file)
			painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignRight | Qt::AlignVCenter, sizeToStr(f.size).c_str());
		break;
	
	case 1:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft | Qt::AlignVCenter, dateToStr(f.time).c_str());
		break;
	
	case 4:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft | Qt::AlignVCenter, permsToStr(f.perm).c_str());
		break;

	default:
		break;
	}
}




void FilesDelegate::drawIcon(QPainter* painter, QRect pos, const files::file_info_t& f) const
{
	QPixmap pixmap;
	switch (f.type)
	{
	case files::file_type::directory_file:
		pixmap = dir.pixmap(QSize(16, 16), QIcon::Normal);
		break;
	default:
		pixmap = fil.pixmap(QSize(16, 16), QIcon::Normal);
		break;
	}
	QPoint p(pos.x(), pos.y() + (pos.height() - 16) / 2);
	painter->drawPixmap(p, pixmap);
}


