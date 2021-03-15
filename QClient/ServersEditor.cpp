#include "ServersEditor.h"

ServersEditor::ServersEditor(ServersModel* model, QWidget *parent)
	: _model(model), AbstractDialogWidget(parent)
{
	ui.setupUi(this);
	_tree = new TreeView(this);
	_tree->setModel(_model);
	_tree->setItemDelegate(new ServerInfoDelegate);
	//_tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
	_tree->setRootIsDecorated(false);
	_model->setColumnTitles({ "description", "Address1", "Address2", "imprint", "type", "port1", "port2" });

	resize(800, 600);

	QObject::connect(_tree->selectionModel(), &QItemSelectionModel::selectionChanged, [this](const QItemSelection& selected, const QItemSelection& deselected) {
		
		_model->update(getState());

		if (selected.indexes().empty())
			return;
		auto it = (TreeItem*)selected.indexes()[0].internalPointer();
		auto serv = it->data<ServerInfo>();
		setState(serv);
		});


	_descriptionWidget = new LineEdit(this);
	_address1Widget = new LineEdit(this);
	_address2Widget = new LineEdit(this);
	_imprintWidget = new LineEdit(this);
	_typeWidget = new LineEdit(this);
	_secretWidget = new QTextEdit(this);
	_port1Widget = new LineEdit(this);
	_port2Widget = new LineEdit(this);


	












	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->setContentsMargins(5, 5, 5, 5);

	{
		QPushButton* btn = new QPushButton("Ok", this);
		btn->setFocusPolicy(Qt::NoFocus);
		buttonsLayout->addWidget(btn);
		QObject::connect(btn, &QPushButton::clicked, [this]() {
			hideDialog();
			//_model->saveState();
		});
	}

	{
		QPushButton* btn = new QPushButton("Add", this);
		btn->setFocusPolicy(Qt::NoFocus);
		buttonsLayout->addWidget(btn);
		QObject::connect(btn, &QPushButton::clicked, [this]() {
			_model->make();
			//updateList();
			//_model->makeItem();
			//_tree->selectionModel()->clearSelection();
			//_tree->selectionModel()->select(_model->index(_model->rowCount(QModelIndex()) - 1), QItemSelectionModel::Rows | QItemSelectionModel::Select);

			});
	}


	{
		QPushButton* btn = new QPushButton("Remove", this);
		btn->setFocusPolicy(Qt::NoFocus);
		buttonsLayout->addWidget(btn);
		QObject::connect(btn, &QPushButton::clicked, [this]() {
			auto indlist = _tree->selectionModel()->selectedIndexes();
			for (size_t i = 0; i < indlist.size(); i++)
			{
				if (indlist[i].column() == 0)
				{
					auto it = (TreeItem*)indlist[i].internalPointer();
					_model->remove(it->data<ServerInfo>().id);
				}

			}

			});
	}
	QWidget* buttonsWidget = new QWidget(this);
	buttonsWidget->setLayout(buttonsLayout);

	

	_formLayout = new QFormLayout(this);
	_formLayout->setMargin(0);
	_formLayout->addRow(tr("description:"), _descriptionWidget);
	_formLayout->addRow(tr("connection type:"), _typeWidget);
	
	_formLayout->addRow(tr("address1:"), _address1Widget);
	_formLayout->addRow(tr("port1:"), _port1Widget);

	_formLayout->addRow(tr("address2:"), _address2Widget);
	_formLayout->addRow(tr("port2:"), _port2Widget);

	_formLayout->addRow(tr("imprint:"), _imprintWidget);

	_formLayout->addRow(tr("secret:"), _secretWidget);
	


	QWidget* formWidget = new QWidget(this);
	formWidget->setLayout(_formLayout);

	QSplitter* splitter = new QSplitter(Qt::Vertical, this);
	
	splitter->addWidget(_tree);
	splitter->addWidget(formWidget);
	
	splitter->setSizes({ 300, 500 });

	QLabel* label = new QLabel(this);
	label->setText("Settings");
	label->setAlignment(Qt::AlignCenter);

	QVBoxLayout* vl = new QVBoxLayout(this);
	setLayout(vl);
	vl->addWidget(label);
	vl->addWidget(splitter);
	vl->addWidget(buttonsWidget);

}

ServersEditor::~ServersEditor()
{
}

void ServersEditor::setServers(std::vector<ServerInfo>& arg)
{
	updateList();
}

void ServersEditor::hideEvent(QHideEvent* event)
{
	_model->update(getState());
	AbstractDialogWidget::hideEvent(event);
}

void ServersEditor::updateList()
{
	
}

void ServersEditor::setState(const ServerInfo& it)
{
	_curent = it;
	_descriptionWidget->setText(it.description.c_str());
	_address1Widget->setText(it.address1.c_str());
	_address2Widget->setText(it.address2.c_str());
	_imprintWidget->setText(it.imprint.c_str());
	_typeWidget->setText(it.connectionType.c_str());

	_secretWidget->setText(it.secret.c_str());

	_port1Widget->setText(std::to_string(it.port1).c_str());
	_port2Widget->setText(std::to_string(it.port2).c_str());
}

ServerInfo ServersEditor::getState()
{
	_curent.description = _descriptionWidget->text().toStdString();
	_curent.address1 = _address1Widget->text().toStdString();
	_curent.address2 = _address2Widget->text().toStdString();
	_curent.imprint = _imprintWidget->text().toStdString();
	_curent.connectionType = _typeWidget->text().toStdString();
	_curent.secret = _secretWidget->toPlainText().toStdString();
	_curent.port1 = (uint16_t)std::atoi(_port1Widget->text().toStdString().c_str());
	_curent.port2 = (uint16_t)std::atoi(_port2Widget->text().toStdString().c_str());

	return _curent;
}














ServerInfoDelegate::ServerInfoDelegate(QObject* parent) : ItemDelegate(parent)
{

}


void ServerInfoDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	painter->save();
	QRect r = option.rect.adjusted(0, 0, 0, 0);

	if (option.state & QStyle::State_MouseOver)
	{
		auto c = option.palette.highlight().color();
		float v = 0.6f;
		painter->fillRect(r, QColor(c.red() * v, c.green() * v, c.blue() * v));
	}


	if (!index.isValid())
		return;


	auto it = (TreeItem*)index.internalPointer();


	if (option.state & QStyle::State_Selected)
	{
		QRect r1 = r;
		r1.setWidth(((QAbstractItemView*)option.widget)->viewport()->width());
		painter->fillRect(r1, option.palette.highlight());
	}

	paintClient(painter, option, index);
	painter->restore();
}



QSize ServerInfoDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return QStyledItemDelegate::sizeHint(option, index);
}



void ServerInfoDelegate::paintClient(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (!index.isValid())
		return;


	QRect r;
	r = option.rect.adjusted(5, 0, -5, 0);

	auto it = (TreeItem*)index.internalPointer();
	ServerInfo serv = it->data<ServerInfo>();

	switch (index.column())
	{
	case 0:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignCenter | Qt::AlignVCenter, serv.description.c_str());
		break;
	case 1:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft | Qt::AlignVCenter, serv.address1.c_str());
		break;
	case 2:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft | Qt::AlignVCenter, serv.address2.c_str());
		break;
	case 3:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft | Qt::AlignVCenter, serv.imprint.c_str());
		break;
	case 4:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft | Qt::AlignVCenter, serv.connectionType.c_str());
		break;
	case 5:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft | Qt::AlignVCenter, std::to_string(serv.port1).c_str());
		break;
	case 6:
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft | Qt::AlignVCenter, std::to_string(serv.port2).c_str());
		break;
	default:
		break;
	}
}
