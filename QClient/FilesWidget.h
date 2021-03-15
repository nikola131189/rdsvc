#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include "ui_FilesWidget.h"
#include "AbstractFilesModel.h"
#include "widgets/TreeView.h"
#include <QSortFilterProxyModel>
#include <QPainter>
#include <sstream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <rd/utility.h>
#include <QInputDialog>
#include <QMenu>
#include <QSettings>
#include <QKeyEvent>
#include <QMouseEvent>
#include "widgets/LineEdit.h"
#include "widgets/InputDialogWidget.h"

class FilesProxyModel;
class FilesDelegate;

class FilesWidget : public QWidget
{
	Q_OBJECT

public:
	FilesWidget(AbstractFilesModel* m, QWidget *parent = Q_NULLPTR);
	~FilesWidget();
	void dir(const std::filesystem::path& p);
	std::filesystem::path path();
	std::list<files::file_info_t> selected();
	void up();
	QByteArray getState();
	void setState(const QByteArray& s);
signals:
	void copy(const std::list<files::file_info_t>& l);
	void openUrl(const std::filesystem::path& p);
	void onMenu(const QPoint& p);
private:
	bool eventFilter(QObject* watched, QEvent* event);
	void initMenu();
	void resizeEvent(QResizeEvent* event);
public:

	void do_remove();
	void do_copy();
	void do_mkdir();
	void do_rename();

	
private:
	AbstractFilesModel* _model;
	TreeView* _tree;
	LineEdit* _line;
	FilesProxyModel* _proxyModel;
	std::filesystem::path _path;
	QMenu* _menu;
private:
	Ui::FilesWidget ui;
};
















struct ci_char_traits : public std::char_traits<wchar_t>
{
	static int compare(const wchar_t* s1, const wchar_t* s2, size_t n) {
		while (n-- != 0) {
			if (std::toupper(*s1) < std::toupper(*s2)) return -1;
			if (std::toupper(*s1) > std::toupper(*s2)) return 1;
			++s1; ++s2;
		}

		return 0;
	}
};


typedef std::basic_string<wchar_t, ci_char_traits> ci_string;




class FilesProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	FilesProxyModel(QObject* parent = 0) : QSortFilterProxyModel(parent) {}

protected:
	//bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
	bool lessThan(const QModelIndex& left, const QModelIndex& right) const override
	{
		auto l = ((TreeItem*)left.internalPointer())->data<files::file_info_t>();
		auto r = ((TreeItem*)right.internalPointer())->data<files::file_info_t>();
		switch (left.column())
		{
		case 0:
			if (l.type == r.type)
				return ci_string(l.path.c_str()) < ci_string(r.path.c_str());
			return l.type > r.type;

		case 2: return ci_string(l.path.extension().c_str()) < ci_string(r.path.extension().c_str());
		
		case 3: return l.size < r.size;

		case 1:
			if (l.type == r.type)
				return l.time < r.time;
			return l.type > r.type;

		case 4: return l.perm < r.perm;
		
		}
		return true;
	}
};


#include "widgets/ItemDelegate.h"

class FilesDelegate : public ItemDelegate
{
	Q_OBJECT
public:
	FilesDelegate(QObject* parent = 0) : ItemDelegate(parent) {}
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
	void paintClient(QPainter* painter, const QRect& rect, const QModelIndex& index) const;
	void drawIcon(QPainter* painter, QRect pos, const files::file_info_t& f) const;
private:
	const QIcon dir = QIcon(QApplication::style()->standardIcon(QStyle::SP_DirIcon));
	const QIcon fil = QIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
};




