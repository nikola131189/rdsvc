#include "StringListWidget.h"

StringListWidget::StringListWidget(const QStringList& data, const QString& title, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	label = new QLabel(title, this);
	list = new QListView(this);
	list->setModel(new QStringListModel(data));
	list->setMouseTracking(true);
	QVBoxLayout* l = new QVBoxLayout(this);
	l->addWidget(label);
	l->addWidget(list);
	setLayout(l);

	QObject::connect(list, &QListView::clicked, [=](const QModelIndex& index)
		{
			this->clicked(index.row(), index.data().toString());
		});
}

StringListWidget::~StringListWidget()
{
}

void StringListWidget::selectRow(int row)
{
	QModelIndex ind = list->model()->index(row, 0);
	list->setCurrentIndex(ind);
	this->clicked(ind.row(), ind.data().toString());
}

void StringListWidget::setData(const QStringList& data)
{
	list->setModel(new QStringListModel(data));
}

int StringListWidget::selectedRow()
{
	QModelIndexList sel = list->selectionModel()->selectedIndexes();
	if (sel.empty()) return 0;

	return sel[0].row();
}

QString StringListWidget::value(int row)
{
	return list->model()->data(list->model()->index(row, 0)).toString();
}
