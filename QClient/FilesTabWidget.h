#pragma once

#include <QWidget>
#include "ui_FilesTabWidget.h"
#include "widgets/TabItem.h"
#include "FilesWidget.h"
#include "FilesModels.h"
#include <rd/rd.h>
#include <QDesktopServices>
#include <QTabWidget>
#include "TransferWidget.h"
#include <QPushButton>
#include <QSplitter>
#include <QButtonGroup>

class FilesTabWidget : public TabItem
{
	Q_OBJECT

public:
	FilesTabWidget(Rd::Client cl, TransferModel* transferModel, QWidget *parent = Q_NULLPTR);
	~FilesTabWidget();
	void getState(QSettings& sett);
	void setState(const QSettings& sett);
private:
	std::filesystem::path replaceParent(const std::filesystem::path& p,
		const std::filesystem::path& old_p, const std::filesystem::path& new_p);

	void upload(const std::list<files::file_info_t>& l);
	void download(const std::list<files::file_info_t>& l);
private:
	Ui::FilesTabWidget ui;

	FilesWidget* _left;
	FilesWidget* _right;

	LocalFilesModel* _leftModel;
	RemoteFilesModel* _rightModel;

	Rd::Client _client;

	TransferWidget* _transferWidget;

	TransferModel* _transferModel;

	QSplitter* _splitter;

	
	QButtonGroup* _btns;
};
