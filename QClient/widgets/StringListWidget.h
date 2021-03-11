#pragma once
#include <QWidget>
#include <QLabel>
#include <QListView>
#include <QVBoxLayout>
#include <QStringListModel>
#include "ui_StringListWidget.h"

class StringListWidget : public QWidget
{
	Q_OBJECT

public:
	StringListWidget(const QStringList& data, const QString& title, QWidget *parent = Q_NULLPTR);
	~StringListWidget();
	void selectRow(int row);
	void setData(const QStringList& data);
	int selectedRow();
	QString value(int row);
signals:
	void clicked(int row, const QString& val);
private:
	QListView* list;
	QLabel* label;
private:
	Ui::StringListWidget ui;
};
